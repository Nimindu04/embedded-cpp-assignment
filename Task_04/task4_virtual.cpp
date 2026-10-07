#include "sensor.hpp"

#include <cassert>
#include <chrono>
#include <cstdint>
#include <iostream>


namespace
{
constexpr uint32_t NumberOfReads = 10000000U;

volatile uint32_t resultSink = 0;


// Prevent the benchmark function from being optimized away.
#if defined(__GNUC__)
__attribute__((noinline))
#endif
uint64_t benchmark(Sensor& sensor)
{
    sensor.init();

    uint32_t total = 0;

    const auto start = std::chrono::steady_clock::now();

    for (uint32_t i = 0; i < NumberOfReads; ++i)
    {
        const std::optional<Sample> sample = sensor.read();

        if (sample.has_value())
        {
            total += sample->value;
        }
    }

    const auto end = std::chrono::steady_clock::now();

    resultSink = total;

    return static_cast<uint64_t>(
        std::chrono::duration_cast<std::chrono::nanoseconds>(
            end - start
        ).count()
    );
}
}


int main()
{
    VirtualTemperatureSensor temperature;
    VirtualPressureSensor pressure;

    Sensor& temperatureSensor = temperature;
    Sensor& pressureSensor = pressure;


    // Check the interface
    temperatureSensor.init();
    pressureSensor.init();

    assert(temperatureSensor.name() != nullptr);
    assert(pressureSensor.name() != nullptr);

    const auto temperatureSample = temperatureSensor.read();
    const auto pressureSample = pressureSensor.read();

    assert(temperatureSample.has_value());
    assert(pressureSample.has_value());

    assert(temperatureSample->value == 251U);
    assert(pressureSample->value == 1001U);


    // Benchmark
    const uint64_t temperatureTime =
        benchmark(temperatureSensor);

    const uint64_t pressureTime =
        benchmark(pressureSensor);


    std::cout << "Virtual sensor benchmark\n";
    std::cout << "------------------------\n";

    std::cout << "Temperature sensor: "
              << temperatureTime
              << " ns\n";

    std::cout << "Pressure sensor:     "
              << pressureTime
              << " ns\n";

    std::cout << "Total reads:         "
              << NumberOfReads
              << " per sensor\n";

    std::cout << "Result sink:         "
              << resultSink
              << '\n';

    return 0;
}