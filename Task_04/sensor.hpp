#ifndef SENSOR_HPP
#define SENSOR_HPP

#include <cstdint>
#include <optional>


// ============================================================
// Common sample type
// ============================================================

struct Sample
{
    uint32_t value;
};


// ============================================================
// Virtual sensor interface
// ============================================================

class Sensor
{
public:
    virtual ~Sensor() = default;

    virtual void init() = 0;

    virtual std::optional<Sample> read() = 0;

    virtual const char* name() const = 0;
};


// ============================================================
// Virtual temperature sensor
// ============================================================

class VirtualTemperatureSensor : public Sensor
{
public:
    void init() override
    {
        value_ = 250;
    }

    std::optional<Sample> read() override
    {
        ++value_;
        return Sample{value_};
    }

    const char* name() const override
    {
        return "Temperature";
    }

private:
    uint32_t value_ = 250;
};


// ============================================================
// Virtual pressure sensor
// ============================================================

class VirtualPressureSensor : public Sensor
{
public:
    void init() override
    {
        value_ = 1000;
    }

    std::optional<Sample> read() override
    {
        ++value_;
        return Sample{value_};
    }

    const char* name() const override
    {
        return "Pressure";
    }

private:
    uint32_t value_ = 1000;
};


// ============================================================
// CRTP sensor base
// ============================================================

template <typename Derived>
class SensorBase
{
public:
    void init()
    {
        derived().initImpl();
    }

    std::optional<Sample> read()
    {
        return derived().readImpl();
    }

    const char* name() const
    {
        return derived().nameImpl();
    }

private:
    Derived& derived()
    {
        return static_cast<Derived&>(*this);
    }

    const Derived& derived() const
    {
        return static_cast<const Derived&>(*this);
    }
};


// ============================================================
// CRTP temperature sensor
// ============================================================

class CrtpTemperatureSensor
    : public SensorBase<CrtpTemperatureSensor>
{
public:
    void initImpl()
    {
        value_ = 250;
    }

    std::optional<Sample> readImpl()
    {
        ++value_;
        return Sample{value_};
    }

    const char* nameImpl() const
    {
        return "Temperature";
    }

private:
    uint32_t value_ = 250;
};


// ============================================================
// CRTP pressure sensor
// ============================================================

class CrtpPressureSensor
    : public SensorBase<CrtpPressureSensor>
{
public:
    void initImpl()
    {
        value_ = 1000;
    }

    std::optional<Sample> readImpl()
    {
        ++value_;
        return Sample{value_};
    }

    const char* nameImpl() const
    {
        return "Pressure";
    }

private:
    uint32_t value_ = 1000;
};

#endif