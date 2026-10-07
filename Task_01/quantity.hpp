#ifndef QUANTITY_HPP
#define QUANTITY_HPP

#include <cstdint>
#include <type_traits>

template <typename Tag, typename Rep = int32_t>
class Quantity
{
public:
    constexpr Quantity() : value_(0) {}

    explicit constexpr Quantity(Rep value)
        : value_(value)
    {
    }

    constexpr Rep value() const
    {
        return value_;
    }

    constexpr Quantity operator+(Quantity other) const
    {
        return Quantity(value_ + other.value_);
    }

    constexpr Quantity operator-(Quantity other) const
    {
        return Quantity(value_ - other.value_);
    }

    constexpr Quantity& operator+=(Quantity other)
    {
        value_ += other.value_;
        return *this;
    }

    constexpr Quantity& operator-=(Quantity other)
    {
        value_ -= other.value_;
        return *this;
    }

    constexpr bool operator==(Quantity other) const
    {
        return value_ == other.value_;
    }

    constexpr bool operator!=(Quantity other) const
    {
        return value_ != other.value_;
    }

    constexpr bool operator<(Quantity other) const
    {
        return value_ < other.value_;
    }

    constexpr bool operator<=(Quantity other) const
    {
        return value_ <= other.value_;
    }

    constexpr bool operator>(Quantity other) const
    {
        return value_ > other.value_;
    }

    constexpr bool operator>=(Quantity other) const
    {
        return value_ >= other.value_;
    }

    constexpr Quantity operator*(Rep scale) const
    {
        return Quantity(value_ * scale);
    }

    constexpr Quantity operator/(Rep scale) const
    {
        return Quantity(value_ / scale);
    }

private:
    Rep value_;
};

template <typename Tag, typename Rep>
constexpr Quantity<Tag, Rep> operator*(Rep scale, Quantity<Tag, Rep> quantity)
{
    return quantity * scale;
}

// Unit tags
struct MillivoltTag {};
struct MilliampTag {};
struct MillisecondTag {};
struct DeciCelsiusTag {};

// Unit types
using Millivolts = Quantity<MillivoltTag>;
using Milliamps = Quantity<MilliampTag>;
using Milliseconds = Quantity<MillisecondTag>;
using DeciCelsius = Quantity<DeciCelsiusTag>;

// User-defined literals
constexpr Millivolts operator""_mV(unsigned long long value)
{
    return Millivolts(static_cast<int32_t>(value));
}

constexpr Milliamps operator""_mA(unsigned long long value)
{
    return Milliamps(static_cast<int32_t>(value));
}

constexpr Milliseconds operator""_ms(unsigned long long value)
{
    return Milliseconds(static_cast<int32_t>(value));
}

constexpr DeciCelsius operator""_dC(unsigned long long value)
{
    return DeciCelsius(static_cast<int32_t>(value));
}

// ADC count to millivolts
constexpr Millivolts adcToMillivolts(uint16_t adcCount)
{
    return Millivolts(
        static_cast<int32_t>((static_cast<uint32_t>(adcCount) * 3300U) / 4095U)
    );
}
static_assert(sizeof(Millivolts) == sizeof(int32_t));
static_assert(sizeof(Milliamps) == sizeof(int32_t));
static_assert(sizeof(Milliseconds) == sizeof(int32_t));
static_assert(sizeof(DeciCelsius) == sizeof(int32_t));

#endif