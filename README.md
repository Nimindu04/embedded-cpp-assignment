# Embedded Internship Assignment — Modern C++ for Firmware

This repository contains my solution for the **Embedded Internship Assignment: Modern C++ for Firmware**.

The assignment focuses on modern C++ features and their application in firmware-oriented software, with an emphasis on type safety, compile-time programming, zero-overhead abstractions, static polymorphism, state machines, and thread-safe concurrency.

## Author

**Name:** Nimindu Prishmika
**University:** University of Moratuwa
**Department:** Electronic and Telecommunication Engineering

---

## Assignment Tasks

### Task 1 — Strong Unit Types

Implemented a compile-time `Quantity` type for strongly typed physical quantities.

Features include:

* Millivolts
* Milliamps
* Milliseconds
* DeciCelsius
* `constexpr` arithmetic
* Compile-time comparisons
* Compile-time scaling
* User-defined literals
* ADC conversion for a 12-bit ADC with a 3.3 V reference
* Compile-time size checks
* Compile-time rejection of operations between incompatible units
* Compiler Explorer comparison demonstrating zero-overhead abstraction

Directory:

```text
Task_01/
```

---

### Task 2 — Compile-Time Register and Field Abstraction

Implemented strongly typed register and field abstractions for firmware-style register access.

Features include:

* Compile-time register fields
* Read and write operations
* Field set and clear operations
* Read-only register protection
* Compile-time field-width validation
* Strongly typed GPIO modes using `enum class`
* Storage policy
* Compiler Explorer comparison demonstrating zero-overhead code generation

Directory:

```text
Task_02/
```

---

### Task 3 — `std::variant` State Machine

Implemented a connection manager using `std::variant` and `std::visit`.

States include:

```text
Idle
Connecting
Connected
Backoff
Error
```

Events include:

```text
Connect
Success
Failure
Timeout
LinkLost
Disconnect
Reset
Tick
```

The implementation includes:

* Exponential backoff
* Backoff sequence of 1, 2, 4, 8, ... seconds
* Maximum backoff of 30 seconds
* Transition to `Error` after the fifth failure
* Reset of failure count after a successful connection
* Ignored-event handling
* Injected clock for deterministic testing
* `std::visit` with overloaded lambdas

Directory:

```text
Task_03/
```

---

### Task 4 — Virtual vs CRTP Sensor Drivers

Implemented and compared two sensor-driver designs:

1. Runtime polymorphism using virtual functions
2. Static polymorphism using CRTP

Both implementations provide:

* `init()`
* `read()`
* `name()`
* `std::optional<Sample>` return type
* Two mock sensors
* Optimized builds using `-Os`
* Runtime benchmark
* Binary-size comparison

The benchmark performs 10 million reads per sensor.

Directory:

```text
Task_04/
```

---

### Task 5 — Thread-Safe Fixed-Capacity Event Bus

Implemented a fixed-capacity, thread-safe event bus for concurrent firmware-style event handling.

Features include:

* Fixed-capacity ring buffer
* `subscribe()`
* `unsubscribe()`
* `publish()`
* `dispatch()`
* Heap-free callbacks using a function pointer and `void*` context
* Mutex-protected shared state
* Non-blocking queue publishing using `try_lock()`
* Queue-full failure reporting
* Multiple producer threads
* Single consumer thread
* Exactly-once event delivery verification
* ThreadSanitizer verification

The stress test uses:

```text
4 producer threads
100,000 events per producer
400,000 total events
```

The final test verified:

```text
Total events: 400000
Delivered: 400000
Duplicates: 0
Exactly-once delivery verified.
```

Directory:

```text
Task_05/
```

---

## Project Structure

```text
embedded-cpp-assignment/
│
├── .gitignore
├── README.md
│
├── Task_01/
│   ├── CMakeLists.txt
│   ├── quantity.hpp
│   └── task1_tests.cpp
│
├── Task_02/
│   ├── CMakeLists.txt
│   ├── register.hpp
│   └── task2_tests.cpp
│
├── Task_03/
│   ├── CMakeLists.txt
│   ├── connection_manager.hpp
│   └── task3_tests.cpp
│
├── Task_04/
│   ├── CMakeLists.txt
│   ├── sensor.hpp
│   ├── task4_virtual.cpp
│   └── task4_crtp.cpp
│
└── Task_05/
    ├── CMakeLists.txt
    ├── event_bus.hpp
    └── task5_tests.cpp
```

---

## Requirements

The project uses:

* C++17
* GCC / G++
* CMake
* Make or MinGW Make
* POSIX threads (`pthread`) for the concurrency task

ThreadSanitizer is used for Task 5 verification.

---

## Building the Tasks

Each task has its own `CMakeLists.txt`.

### Task 1

```bash
cmake -S Task_01 -B Task_01/build
cmake --build Task_01/build
```

Run:

```bash
./Task_01/build/task1_tests
```

### Task 2

```bash
cmake -S Task_02 -B Task_02/build
cmake --build Task_02/build
```

Run:

```bash
./Task_02/build/task2_tests
```

### Task 3

```bash
cmake -S Task_03 -B Task_03/build
cmake --build Task_03/build
```

Run:

```bash
./Task_03/build/task3_tests
```

### Task 4

```bash
cmake -S Task_04 -B Task_04/build
cmake --build Task_04/build
```

The Task 4 executables can then be run to reproduce the virtual and CRTP benchmarks.

### Task 5

```bash
cmake -S Task_05 -B Task_05/build
cmake --build Task_05/build
```

Run:

```bash
./Task_05/build/task5_tests
```

---

## ThreadSanitizer — Task 5

Task 5 was also built and tested with GCC ThreadSanitizer.

Example configuration:

```bash
cmake -S Task_05 -B build-tsan \
    -G "Unix Makefiles" \
    -DCMAKE_CXX_FLAGS="-fsanitize=thread -g" \
    -DCMAKE_EXE_LINKER_FLAGS="-fsanitize=thread"
```

Build:

```bash
cmake --build build-tsan
```

Run:

```bash
./build-tsan/task5_tests
```

The stress test completed successfully without ThreadSanitizer data-race warnings.

---

## Testing Summary

| Task   | Main Verification                                           |
| ------ | ----------------------------------------------------------- |
| Task 1 | Unit-type tests and compile-time checks                     |
| Task 2 | Register/field tests and compile-time error checks          |
| Task 3 | State-machine transition tests                              |
| Task 4 | Benchmark and binary-size comparison                        |
| Task 5 | Queue-full test, 400,000-event stress test, ThreadSanitizer |

---

## Report

A detailed report is provided separately and contains:

* Design decisions
* Implementation details
* Compiler Explorer comparisons for Tasks 1 and 2
* Task 4 benchmark and binary-size results
* Task 5 concurrency and ThreadSanitizer results
* Test evidence and screenshots
