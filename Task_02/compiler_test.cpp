#include "register.hpp"

void writeRaw(uint32_t& reg, uint32_t value)
{
    constexpr uint32_t mask = 0xF00U;

    reg = (reg & ~mask) | ((value << 8) & mask);
}

void writeField(uint32_t& reg, uint32_t value)
{
    ArrayStorage<uint32_t> storage(reg);
    Register<ArrayStorage<uint32_t>> registerObject(storage);

    Field<Register<ArrayStorage<uint32_t>>, 8, 4> field(registerObject);

    field.write(value);
}