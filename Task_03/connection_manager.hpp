#ifndef CONNECTION_MANAGER_HPP
#define CONNECTION_MANAGER_HPP

#include <cstdint>
#include <iostream>
#include <type_traits>
#include <variant>


// ============================================================
// Clock interface
// ============================================================

class IClock
{
public:
    virtual ~IClock() = default;

    virtual uint32_t now() const = 0;
};


// ============================================================
// Test clock
// ============================================================

class TestClock : public IClock
{
public:
    explicit TestClock(uint32_t initialTime = 0)
        : currentTime_(initialTime)
    {
    }

    uint32_t now() const override
    {
        return currentTime_;
    }

    void advance(uint32_t seconds)
    {
        currentTime_ += seconds;
    }

private:
    uint32_t currentTime_;
};


// ============================================================
// States
// ============================================================

struct Idle
{
};

struct Connecting
{
    uint32_t attempt;
};

struct Connected
{
    uint32_t since;
};

struct Backoff
{
    uint32_t until;
    uint32_t failures;
};

struct Error
{
    uint32_t code;
};


// ============================================================
// Events
// ============================================================

struct Connect
{
};

struct Success
{
};

struct Failure
{
};

struct Timeout
{
};

struct LinkLost
{
};

struct Disconnect
{
};

struct Reset
{
};

struct Tick
{
    uint32_t now;
};


// ============================================================
// State variant
// ============================================================

using ConnectionState = std::variant<
    Idle,
    Connecting,
    Connected,
    Backoff,
    Error
>;


// ============================================================
// Overloaded lambda helper
// ============================================================

template <typename... Ts>
struct Overloaded : Ts...
{
    using Ts::operator()...;
};

template <typename... Ts>
Overloaded(Ts...) -> Overloaded<Ts...>;


// ============================================================
// Event name helpers
// ============================================================

inline const char* eventName(const Connect&)
{
    return "Connect";
}

inline const char* eventName(const Success&)
{
    return "Success";
}

inline const char* eventName(const Failure&)
{
    return "Failure";
}

inline const char* eventName(const Timeout&)
{
    return "Timeout";
}

inline const char* eventName(const LinkLost&)
{
    return "LinkLost";
}

inline const char* eventName(const Disconnect&)
{
    return "Disconnect";
}

inline const char* eventName(const Reset&)
{
    return "Reset";
}

inline const char* eventName(const Tick&)
{
    return "Tick";
}


// ============================================================
// Connection Manager
// ============================================================

class ConnectionManager
{
public:
    static constexpr uint32_t MaxFailures = 5;
    static constexpr uint32_t MaxBackoffSeconds = 30;
    static constexpr uint32_t ErrorCodeMaxFailures = 5;

    explicit ConnectionManager(IClock& clock)
        : clock_(clock),
          state_(Idle{})
    {
    }


    template <typename Event>
    void handle(const Event& event)
    {
        std::visit(
            Overloaded{
                [this, &event](Idle& state)
                {
                    handleState(state, event);
                },

                [this, &event](Connecting& state)
                {
                    handleState(state, event);
                },

                [this, &event](Connected& state)
                {
                    handleState(state, event);
                },

                [this, &event](Backoff& state)
                {
                    handleState(state, event);
                },

                [this, &event](Error& state)
                {
                    handleState(state, event);
                }
            },
            state_
        );
    }


    const ConnectionState& state() const
    {
        return state_;
    }


private:

    static uint32_t calculateBackoff(uint32_t failures)
    {
        uint32_t delay = 1;

        for (uint32_t i = 1; i < failures; ++i)
        {
            if (delay >= MaxBackoffSeconds)
            {
                break;
            }

            delay *= 2;

            if (delay > MaxBackoffSeconds)
            {
                delay = MaxBackoffSeconds;
            }
        }

        return delay;
    }


    void enterBackoff(uint32_t failures)
    {
        if (failures >= MaxFailures)
        {
            state_ = Error{ErrorCodeMaxFailures};
            return;
        }

        const uint32_t delay = calculateBackoff(failures);

        state_ = Backoff{
            clock_.now() + delay,
            failures
        };
    }


    template <typename Event>
    void logIgnored(const char* stateName, const Event& event)
    {
        std::cout
            << "Ignored event "
            << eventName(event)
            << " in state "
            << stateName
            << '\n';
    }


    // --------------------------------------------------------
    // Idle
    // --------------------------------------------------------

    void handleState(Idle&, const Connect&)
    {
        state_ = Connecting{1};
    }

    template <typename Event>
    void handleState(Idle& state, const Event& event)
    {
        static_cast<void>(state);
        logIgnored("Idle", event);
    }


    // --------------------------------------------------------
    // Connecting
    // --------------------------------------------------------

    void handleState(Connecting&, const Success&)
    {
        state_ = Connected{clock_.now()};
    }

    void handleState(Connecting& state, const Failure&)
    {
        enterBackoff(state.attempt);
    }

    void handleState(Connecting& state, const Timeout&)
    {
        enterBackoff(state.attempt);
    }

    template <typename Event>
    void handleState(Connecting& state, const Event& event)
    {
        static_cast<void>(state);
        logIgnored("Connecting", event);
    }


    // --------------------------------------------------------
    // Connected
    // --------------------------------------------------------

    void handleState(Connected&, const LinkLost&)
    {
        enterBackoff(1);
    }

    void handleState(Connected&, const Disconnect&)
    {
        state_ = Idle{};
    }

    template <typename Event>
    void handleState(Connected& state, const Event& event)
    {
        static_cast<void>(state);
        logIgnored("Connected", event);
    }


    // --------------------------------------------------------
    // Backoff
    // --------------------------------------------------------

    void handleState(Backoff& state, const Tick& event)
    {
        if (event.now >= state.until)
        {
            state_ = Connecting{state.failures + 1};
        }
    }

    template <typename Event>
    void handleState(Backoff& state, const Event& event)
    {
        static_cast<void>(state);
        logIgnored("Backoff", event);
    }


    // --------------------------------------------------------
    // Error
    // --------------------------------------------------------

    void handleState(Error&, const Reset&)
    {
        state_ = Idle{};
    }

    template <typename Event>
    void handleState(Error& state, const Event& event)
    {
        static_cast<void>(state);
        logIgnored("Error", event);
    }


private:
    IClock& clock_;
    ConnectionState state_;
};

#endif