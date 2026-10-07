#include "quantity.hpp"

#include <cassert>

int main()
{
    // Addition
    constexpr Millivolts v1 = Millivolts(1000);
    constexpr Millivolts v2 = Millivolts(500);


    static_assert(v3.value() == 1500);

    // Subtraction
    constexpr auto v4 = v1 - v2;

    static_assert(v4.value() == 500);

    // Comparison
    static_assert(v1 > v2);
    static_assert(v2 < v1);
    static_assert(v1 != v2);
    static_assert(v1 == Millivolts(1000));

    // Scaling
    constexpr auto v5 = v2 * 3;

    static_assert(v5.value() == 1500);

    constexpr auto v6 = 3 * v2;

    static_assert(v6.value() == 1500);

    // User-defined literals
    constexpr auto v7 = 3300_mV;
    constexpr auto time = 250_ms;

    static_assert(v7.value() == 3300);
    static_assert(time.value() == 250);

    // ADC conversion
    constexpr auto zero = adcToMillivolts(0);
    constexpr auto maximum = adcToMillivolts(4095);

    static_assert(zero.value() == 0);
    static_assert(maximum.value() == 3300);

    // Runtime checks
    Milliamps current(100);

    assert(current.value() == 100);

    Milliseconds delay(250);

    assert(delay.value() == 250);

#if 0

// Different units cannot be added
auto error1 = 3300_mV + 250_ms;

// Different units cannot be subtracted
auto error2 = 3300_mV - 100_mA;

// Different units cannot be compared
auto error3 = 3300_mV > 100_mA;

// Millivolts cannot be assigned to Milliamps
Milliamps error4 = 3300_mV;

#endif    


    return 0;
}