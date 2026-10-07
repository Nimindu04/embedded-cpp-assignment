#include "event_bus.hpp"

#include <atomic>
#include <cassert>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <thread>


namespace
{

constexpr EventId TestEventId = 1U;

constexpr uint32_t ProducerCount = 4U;
constexpr uint32_t EventsPerProducer = 100000U;
constexpr uint32_t TotalEvents =
    ProducerCount * EventsPerProducer;


// ============================================================
// Stress-test context
// ============================================================

struct StressContext
{
    std::atomic<uint32_t> delivered{0};
    std::atomic<uint32_t> duplicates{0};

    std::array<std::atomic<uint32_t>, TotalEvents> seen;

    StressContext()
    {
        for (auto& value : seen)
        {
            value.store(0U);
        }
    }
};


// ============================================================
// Stress-test callback
// ============================================================

void stressCallback(const Event& event, void* context)
{
    auto* stress =
        static_cast<StressContext*>(context);

    if (event.value >= TotalEvents)
    {
        return;
    }

    const uint32_t previous =
        stress->seen[event.value].fetch_add(
            1U,
            std::memory_order_relaxed
        );

    if (previous != 0U)
    {
        stress->duplicates.fetch_add(
            1U,
            std::memory_order_relaxed
        );
    }

    stress->delivered.fetch_add(
        1U,
        std::memory_order_relaxed
    );
}

}


// ============================================================
// Main
// ============================================================

int main()
{
    using Bus = EventBus<4096, 4>;
    // Queue-full test
    {
        Bus fullBus;

        Event event{
            TestEventId,
            123U
        };

        uint32_t successfulPublishes = 0U;

        while (fullBus.publish(event))
        {
            ++successfulPublishes;
        }

        assert(successfulPublishes == 4095U);

        std::cout
            << "Queue-full test passed. "
            << "Publish correctly returned false when full.\n";
    }
    
    Bus bus;

    static StressContext context;


    // --------------------------------------------------------
    // Subscribe
    // --------------------------------------------------------

    const Bus::Handle handle =
        bus.subscribe(
            TestEventId,
            stressCallback,
            &context
        );

    assert(handle != Bus::InvalidHandle);


    // --------------------------------------------------------
    // Producer threads
    // --------------------------------------------------------

    std::array<std::thread, ProducerCount> producers;


    for (uint32_t producer = 0;
         producer < ProducerCount;
         ++producer)
    {
        producers[producer] =
            std::thread(
                [&bus, producer]()
                {
                    const uint32_t start =
                        producer * EventsPerProducer;

                    const uint32_t end =
                        start + EventsPerProducer;

                    for (uint32_t value = start;
                         value < end;
                         ++value)
                    {
                        Event event{
                            TestEventId,
                            value
                        };


                        /*
                         * publish() is allowed to fail because
                         * the queue may be full or another
                         * producer may currently own the mutex.
                         *
                         * Retry until this event is accepted.
                         */

                        while (!bus.publish(event))
                        {
                            std::this_thread::yield();
                        }
                    }
                }
            );
    }


    // --------------------------------------------------------
    // Consumer thread
    // --------------------------------------------------------

    std::thread consumer(
        [&bus]()
        {
            while (
                context.delivered.load(
                    std::memory_order_relaxed
                ) < TotalEvents
            )
            {
                if (!bus.dispatch())
                {
                    std::this_thread::yield();
                }
            }
        }
    );


    // --------------------------------------------------------
    // Wait for producers
    // --------------------------------------------------------

    for (std::thread& producer : producers)
    {
        producer.join();
    }


    // --------------------------------------------------------
    // Wait for consumer
    // --------------------------------------------------------

    consumer.join();


    // --------------------------------------------------------
    // Verify exactly-once delivery
    // --------------------------------------------------------

    assert(
        context.delivered.load() ==
        TotalEvents
    );

    assert(
        context.duplicates.load() == 0U
    );


    for (const auto& count : context.seen)
    {
        assert(count.load() == 1U);
    }


    // --------------------------------------------------------
    // Test unsubscribe
    // --------------------------------------------------------

    assert(bus.unsubscribe(handle));

    assert(
        !bus.unsubscribe(handle)
    );


    std::cout
        << "Task 5 stress test passed.\n";

    std::cout
        << "Total events: "
        << TotalEvents
        << '\n';

    std::cout
        << "Delivered: "
        << context.delivered.load()
        << '\n';

    std::cout
        << "Duplicates: "
        << context.duplicates.load()
        << '\n';

    std::cout
        << "Exactly-once delivery verified.\n";


    return 0;
}