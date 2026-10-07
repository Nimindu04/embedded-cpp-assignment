#include "connection_manager.hpp"

#include <cassert>
#include <cstdint>
#include <iostream>
#include <variant>


int main()
{
    TestClock clock(0);

    ConnectionManager manager(clock);


    // ========================================================
    // Idle -> Connecting
    // ========================================================

    assert(std::holds_alternative<Idle>(manager.state()));

    manager.handle(Connect{});

    assert(std::holds_alternative<Connecting>(manager.state()));

    const auto& connecting1 =
        std::get<Connecting>(manager.state());

    assert(connecting1.attempt == 1);


    // ========================================================
    // Connecting -> Connected
    // ========================================================

    clock.advance(10);

    manager.handle(Success{});

    assert(std::holds_alternative<Connected>(manager.state()));

    const auto& connected =
        std::get<Connected>(manager.state());

    assert(connected.since == 10);


    // ========================================================
    // Connected -> Idle
    // ========================================================

    manager.handle(Disconnect{});

    assert(std::holds_alternative<Idle>(manager.state()));


    // ========================================================
    // Idle -> Connecting again
    // ========================================================

    manager.handle(Connect{});

    assert(std::holds_alternative<Connecting>(manager.state()));

    assert(
        std::get<Connecting>(manager.state()).attempt == 1
    );


    // ========================================================
    // Connecting -> Backoff using Failure
    // ========================================================

    manager.handle(Failure{});

    assert(std::holds_alternative<Backoff>(manager.state()));

    const auto& backoff1 =
        std::get<Backoff>(manager.state());

    assert(backoff1.failures == 1);
    assert(backoff1.until == 11);


    // ========================================================
    // Backoff -> Connecting
    // ========================================================

    clock.advance(1);

    manager.handle(Tick{clock.now()});

    assert(std::holds_alternative<Connecting>(manager.state()));

    assert(
        std::get<Connecting>(manager.state()).attempt == 2
    );


    // ========================================================
    // Connecting -> Backoff using Timeout
    // ========================================================

    manager.handle(Timeout{});

    assert(std::holds_alternative<Backoff>(manager.state()));

    const auto& backoff2 =
        std::get<Backoff>(manager.state());

    assert(backoff2.failures == 2);
    assert(backoff2.until == 13);


    // ========================================================
    // Backoff -> Connecting
    // ========================================================

    clock.advance(2);

    manager.handle(Tick{clock.now()});

    assert(std::holds_alternative<Connecting>(manager.state()));

    assert(
        std::get<Connecting>(manager.state()).attempt == 3
    );


    // ========================================================
    // Third failure
    // ========================================================

    manager.handle(Failure{});

    assert(std::holds_alternative<Backoff>(manager.state()));

    const auto& backoff3 =
        std::get<Backoff>(manager.state());

    assert(backoff3.failures == 3);
    assert(backoff3.until == 17);


    // ========================================================
    // Fourth failure
    // ========================================================

    clock.advance(4);

    manager.handle(Tick{clock.now()});

    assert(std::holds_alternative<Connecting>(manager.state()));

    assert(
        std::get<Connecting>(manager.state()).attempt == 4
    );

    manager.handle(Failure{});

    assert(std::holds_alternative<Backoff>(manager.state()));

    const auto& backoff4 =
        std::get<Backoff>(manager.state());

    assert(backoff4.failures == 4);
    assert(backoff4.until == 25);


    // ========================================================
    // Fifth failure -> Error
    // ========================================================

    clock.advance(8);

    manager.handle(Tick{clock.now()});

    assert(std::holds_alternative<Connecting>(manager.state()));

    assert(
        std::get<Connecting>(manager.state()).attempt == 5
    );

    manager.handle(Failure{});

    assert(std::holds_alternative<Error>(manager.state()));

    const auto& error =
        std::get<Error>(manager.state());

    assert(error.code == ConnectionManager::ErrorCodeMaxFailures);


    // ========================================================
    // Error -> Idle
    // ========================================================

    manager.handle(Reset{});

    assert(std::holds_alternative<Idle>(manager.state()));


    // ========================================================
    // Test Connected -> Backoff using LinkLost
    // ========================================================

    manager.handle(Connect{});

    assert(std::holds_alternative<Connecting>(manager.state()));

    manager.handle(Success{});

    assert(std::holds_alternative<Connected>(manager.state()));

    manager.handle(LinkLost{});

    assert(std::holds_alternative<Backoff>(manager.state()));

    const auto& linkLostBackoff =
        std::get<Backoff>(manager.state());

    assert(linkLostBackoff.failures == 1);


    // ========================================================
    // Test Success resets the failure count
    // ========================================================

    clock.advance(1);

    manager.handle(Tick{clock.now()});

    assert(std::holds_alternative<Connecting>(manager.state()));

    manager.handle(Success{});

    assert(std::holds_alternative<Connected>(manager.state()));


    // Link lost after successful connection starts at failure 1
    manager.handle(LinkLost{});

    assert(std::holds_alternative<Backoff>(manager.state()));

    assert(
        std::get<Backoff>(manager.state()).failures == 1
    );


    // ========================================================
    // Test ignored events
    // ========================================================

    // Connect does not make sense in Backoff
    manager.handle(Connect{});

    assert(std::holds_alternative<Backoff>(manager.state()));

    // Reset does not make sense in Backoff
    manager.handle(Reset{});

    assert(std::holds_alternative<Backoff>(manager.state()));


    // ========================================================
    // Backoff cap test
    // ========================================================

    clock.advance(1);

    manager.handle(Tick{clock.now()});

    assert(std::holds_alternative<Connecting>(manager.state()));

    manager.handle(Failure{});

    assert(std::holds_alternative<Backoff>(manager.state()));

    const auto& cappedBackoff =
        std::get<Backoff>(manager.state());

    assert(cappedBackoff.failures == 2);


    std::cout << "All Task 3 tests passed.\n";

    return 0;
}