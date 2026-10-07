#include "register.hpp"

#include <cassert>
#include <cstdint>

int main()
{
    // Test storage
    uint32_t memory = 0;

    ArrayStorage<uint32_t> storage(memory);

    // Writable register
    Register<ArrayStorage<uint32_t>> reg(storage);

    // Basic register read/write
    reg.write(0x12345678U);

    assert(reg.read() == 0x12345678U);


    // 1-bit field
    Field<Register<ArrayStorage<uint32_t>>, 3, 1> enable(reg);

    enable.set();

    assert(enable.read() == 1U);

    enable.clear();

    assert(enable.read() == 0U);


    // Multi-bit field
    Field<Register<ArrayStorage<uint32_t>>, 8, 4> field(reg);

    field.write(5);

    assert(field.read() == 5U);


    // GPIO mode field
    GpioModeField<Register<ArrayStorage<uint32_t>>, 4> mode(reg);

    mode.write(GpioMode::Output);

    assert(mode.read() == GpioMode::Output);

    mode.write(GpioMode::Analog);

    assert(mode.read() == GpioMode::Analog);


    // Read-only register
    uint32_t readOnlyMemory = 0;

    ArrayStorage<uint32_t> readOnlyStorage(readOnlyMemory);

    Register<ArrayStorage<uint32_t>, true> readOnlyReg(readOnlyStorage);

    assert(readOnlyReg.read() == 0U);


    return 0;
}