#ifndef REGISTER_HPP
#define REGISTER_HPP

#include <cstdint>
#include <type_traits>

// Storage policy for testing with normal variables
template <typename Storage>
class ArrayStorage
{
public:
    constexpr explicit ArrayStorage(Storage& storage)
        : storage_(storage)
    {
    }

    constexpr uint32_t read() const
    {
        return storage_;
    }

    constexpr void write(uint32_t value)
    {
        storage_ = value;
    }

private:
    Storage& storage_;
};


// Register
template <typename Storage, bool ReadOnly = false>
class Register
{
public:
    constexpr explicit Register(Storage& storage)
        : storage_(storage)
    {
    }

    constexpr uint32_t read() const
    {
        return storage_.read();
    }

    template <bool RO = ReadOnly>
    constexpr void write(uint32_t value)
    {
        static_assert(!RO,
                      "Cannot write to a read-only register");

        storage_.write(value);
    }

private:
    Storage& storage_;
};


// Register field
template <typename RegisterType, unsigned Position, unsigned Width>
class Field
{
    static_assert(Position < 32,
                  "Field position must be less than 32");

    static_assert(Width > 0,
                  "Field width must be greater than zero");

    static_assert(Position + Width <= 32,
                  "Field must fit within 32 bits");

public:
    constexpr explicit Field(RegisterType& reg)
        : reg_(reg)
    {
    }

    constexpr uint32_t read() const
    {
        constexpr uint32_t mask =
            ((uint32_t{1} << Width) - 1U) << Position;

        return (reg_.read() & mask) >> Position;
    }

    constexpr void write(uint32_t value)
    {
        constexpr uint32_t mask =
            ((uint32_t{1} << Width) - 1U) << Position;

        const uint32_t current = reg_.read();

        reg_.write(
            (current & ~mask) |
            ((value << Position) & mask)
        );
    }

    template <unsigned W = Width>
    constexpr void set()
    {
        static_assert(W == 1,
                      "set() is only available for 1-bit fields");

        reg_.write(
            reg_.read() | (uint32_t{1} << Position)
        );
    }

    template <unsigned W = Width>
    constexpr void clear()
    {
        static_assert(W == 1,
                      "clear() is only available for 1-bit fields");

        reg_.write(
            reg_.read() & ~(uint32_t{1} << Position)
        );
    }

private:
    RegisterType& reg_;
};


// Strongly typed GPIO mode
enum class GpioMode : uint32_t
{
    Input   = 0,
    Output  = 1,
    AltFunc = 2,
    Analog  = 3
};


// Strongly typed GPIO mode field
template <typename RegisterType, unsigned Position>
class GpioModeField
{
    static_assert(Position + 2 <= 32,
                  "GPIO mode field must fit within 32 bits");

public:
    constexpr explicit GpioModeField(RegisterType& reg)
        : reg_(reg)
    {
    }

    constexpr GpioMode read() const
    {
        constexpr uint32_t mask = 0x3U << Position;

        return static_cast<GpioMode>(
            (reg_.read() & mask) >> Position
        );
    }

    constexpr void write(GpioMode mode)
    {
        constexpr uint32_t mask = 0x3U << Position;

        const uint32_t value =
            static_cast<uint32_t>(mode);

        reg_.write(
            (reg_.read() & ~mask) |
            ((value << Position) & mask)
        );
    }

private:
    RegisterType& reg_;
};


// Size checks
static_assert(sizeof(uint32_t) == 4,
              "uint32_t must be 32 bits");

#endif