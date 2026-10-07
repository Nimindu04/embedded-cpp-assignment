#ifndef EVENT_BUS_HPP
#define EVENT_BUS_HPP

#include <array>
#include <cstddef>
#include <cstdint>
#include <mutex>


// ============================================================
// Event types
// ============================================================

using EventId = uint32_t;

struct Event
{
    EventId id;
    uint32_t value;
};


// ============================================================
// Heap-free callback
// ============================================================

using Callback = void (*)(const Event&, void*);


// ============================================================
// EventBus
// ============================================================

template <std::size_t QueueCapacity,
          std::size_t MaxSubscribers>
class EventBus
{
private:

    struct QueuedEvent
    {
        Event event;
    };


    struct Subscriber
    {
        EventId id = 0;
        Callback callback = nullptr;
        void* context = nullptr;
        uint32_t generation = 0;
        bool active = false;
    };


public:

    using Handle = uint32_t;

    static constexpr Handle InvalidHandle = 0;


    EventBus()
        : head_(0),
          tail_(0),
          nextHandle_(1)
    {
    }


    // --------------------------------------------------------
    // Subscribe
    // --------------------------------------------------------

    Handle subscribe(EventId id,
                     Callback callback,
                     void* context)
    {
        if (callback == nullptr)
        {
            return InvalidHandle;
        }

        std::lock_guard<std::mutex> lock(subscriberMutex_);

        for (std::size_t i = 0; i < MaxSubscribers; ++i)
        {
            Subscriber& subscriber = subscribers_[i];

            if (!subscriber.active)
            {
                subscriber.id = id;
                subscriber.callback = callback;
                subscriber.context = context;
                subscriber.generation = nextHandle_;
                subscriber.active = true;

                const Handle handle = nextHandle_;

                ++nextHandle_;

                if (nextHandle_ == InvalidHandle)
                {
                    ++nextHandle_;
                }

                return handle;
            }
        }

        return InvalidHandle;
    }


    // --------------------------------------------------------
    // Unsubscribe
    // --------------------------------------------------------

    bool unsubscribe(Handle handle)
    {
        if (handle == InvalidHandle)
        {
            return false;
        }

        std::lock_guard<std::mutex> lock(subscriberMutex_);

        for (Subscriber& subscriber : subscribers_)
        {
            if (subscriber.active &&
                subscriber.generation == handle)
            {
                subscriber.active = false;
                subscriber.callback = nullptr;
                subscriber.context = nullptr;

                return true;
            }
        }

        return false;
    }


    // --------------------------------------------------------
    // Publish
    // --------------------------------------------------------

    bool publish(const Event& event)
    {
        /*
         * try_lock() is used deliberately.
         *
         * publish() never waits for another thread holding the
         * queue mutex. If the mutex is busy, the caller gets a
         * failure and can retry.
         *
         * The critical section only copies one Event into the
         * fixed-capacity queue.
         */

        if (!queueMutex_.try_lock())
        {
            return false;
        }

        const std::size_t next =
            (head_ + 1U) % QueueCapacity;

        if (next == tail_)
        {
            queueMutex_.unlock();
            return false;
        }

        queue_[head_].event = event;

        head_ = next;

        queueMutex_.unlock();

        return true;
    }


    // --------------------------------------------------------
    // Dispatch
    // --------------------------------------------------------

    bool dispatch()
    {
        Event event;

        {
            std::lock_guard<std::mutex> lock(queueMutex_);

            if (tail_ == head_)
            {
                return false;
            }

            event = queue_[tail_].event;

            tail_ = (tail_ + 1U) % QueueCapacity;
        }


        /*
         * The queue mutex is released before callbacks are
         * executed. Therefore a callback cannot block producers.
         */

        std::array<Subscriber, MaxSubscribers> snapshot{};
        std::size_t count = 0;

        {
            std::lock_guard<std::mutex> lock(subscriberMutex_);

            for (const Subscriber& subscriber : subscribers_)
            {
                if (subscriber.active &&
                    subscriber.id == event.id)
                {
                    snapshot[count] = subscriber;
                    ++count;
                }
            }
        }


        for (std::size_t i = 0; i < count; ++i)
        {
            snapshot[i].callback(
                event,
                snapshot[i].context
            );
        }

        return true;
    }


    // --------------------------------------------------------
    // Queue state
    // --------------------------------------------------------

    bool empty() const
    {
        std::lock_guard<std::mutex> lock(queueMutex_);

        return head_ == tail_;
    }


private:

    std::array<QueuedEvent, QueueCapacity> queue_{};

    mutable std::mutex queueMutex_;

    std::size_t head_;
    std::size_t tail_;


    std::array<Subscriber, MaxSubscribers> subscribers_{};

    mutable std::mutex subscriberMutex_;

    Handle nextHandle_;
};

#endif