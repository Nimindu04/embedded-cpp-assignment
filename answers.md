# Part A

## Object Lifetime and Resources

### 1. Rule of Zero, Three and Five

The Rule of Zero means a class should avoid manually managing resources and use classes that manage them automatically.

The Rule of Three says that if a class needs a custom destructor, copy constructor, or copy assignment operator, it usually needs all three.

The Rule of Five adds a move constructor and move assignment operator.

Most classes should follow the Rule of Zero because it is simpler and reduces resource-management bugs.

```cpp
class Data {
    std::array<int, 10> values;
};
```

---

### 2. What does `std::move()` do?

`std::move()` does not actually move an object. It allows the object to be treated as an rvalue so that a move operation can be selected.

```cpp
std::unique_ptr<int> a = std::make_unique<int>(10);
std::unique_ptr<int> b = std::move(a);
```

After the move, `a` is guaranteed to be empty (`nullptr`).

For a user-defined type, the moved-from object is normally valid but has an unspecified state.

---

### 3. Why should move constructors be `noexcept`?

A move constructor should be `noexcept` when it cannot throw an exception.

This allows standard library containers such as `std::vector` to prefer moving objects instead of copying them when they need to reallocate.

```cpp
Buffer(Buffer&& other) noexcept;
```

This can improve performance and maintain exception-safety guarantees.

---

### 4. When must a base class have a virtual destructor?

A base class should have a virtual destructor when derived objects may be deleted through a base-class pointer.

```cpp
class Device {
public:
    virtual ~Device() = default;
};
```

Without a virtual destructor, deleting a derived object through a base pointer can cause undefined behavior.

---

### 5. Static initialization order problem

The problem occurs when global objects in different source files depend on each other. Their initialization order is not guaranteed.

Two ways to avoid it are:

1. Use a function-local static object.
2. Create objects explicitly in the required order, such as in `main()`.

```cpp
Device& getDevice()
{
    static Device device;
    return device;
}
```

---

# Polymorphism and Templates

### 6. How does a vtable work?

A vtable is a table used to implement virtual functions.

A polymorphic object normally contains a hidden pointer called a `vptr` that points to the appropriate vtable.

When a virtual function is called, the program uses the vtable to find the correct function at runtime.

The exact RAM and flash overhead depends on the compiler, architecture, ABI, and optimization settings.

---

### 7. Static vs dynamic polymorphism

Static polymorphism uses templates or CRTP and is resolved at compile time.

Dynamic polymorphism uses virtual functions and is resolved at runtime.

Static polymorphism usually has lower runtime overhead but can increase code size and compile time.

Dynamic polymorphism is more flexible and is useful when different types need to be handled through a common interface at runtime.

---

### 8. Why do template definitions usually live in header files?

The compiler needs to see the template definition when it creates an instance for a particular type.

Therefore, template definitions are usually placed in header files.

**Code bloat** means that multiple template instantiations can generate multiple versions of similar code.

It can be reduced by avoiding unnecessary template instantiations and sharing common non-template code.

---

### 9. `const`, `constexpr`, and `consteval`

`const` means the value cannot be modified after initialization.

`constexpr` allows a value or function to be evaluated at compile time when possible.

`consteval` is a C++20 feature that requires a function to be evaluated at compile time.

```cpp
constexpr int square(int x)
{
    return x * x;
}
```

`constexpr` is useful when the same function may be used with both compile-time and runtime values.

---

### 10. Why use `enum class` for register fields?

`enum class` provides strong type safety.

```cpp
enum class GpioMode {
    Input,
    Output,
    AltFunc,
    Analog
};
```

It prevents accidental use of unrelated integers and makes register-field values clearer and safer.

---

# Errors, Memory and Concurrency

### 11. How can errors be reported without exceptions?

Errors can be reported using:

* Return codes
* `std::optional`
* `std::variant`
* `std::expected` in C++23

A return code is simple and common in firmware.

`std::optional` is useful when there is either a value or no value.

`std::variant` can represent different result types.

`std::expected` represents either a successful value or an error.

---

### 12. What does `std::function` cost?

`std::function` provides a flexible way to store different callable objects, but it has some runtime and memory overhead.

It can allocate memory when the stored callable does not fit into its internal storage.

A heap-free alternative is a function pointer with a `void*` context:

```cpp
using Callback = void (*)(void*, int);
```

This is more predictable for embedded systems.

---

### 13. Is `reinterpret_cast<Packet*>(rx_buffer)` safe?

Not necessarily.

The buffer may have incorrect alignment, and the bytes are not automatically a valid `Packet` object.

A safer approach is to copy the bytes into an actual object:

```cpp
Packet packet{};
std::memcpy(&packet, rx_buffer, sizeof(packet));
```

Packet size, padding, byte order, and data validity should also be considered.

---

### 14. Why is `volatile` not a substitute for `std::atomic`?

`volatile` tells the compiler that a value can change unexpectedly. It is commonly useful for hardware registers.

However, `volatile` does not provide thread synchronization or atomic operations.

`std::atomic` is designed for safe communication between threads.

```cpp
std::atomic<bool> ready{false};
```

`memory_order_release` ensures previous writes are ordered before the release operation.

`memory_order_acquire` ensures that a thread observing the released value can see those earlier writes.

---

### 15. What is placement new?

Placement new constructs an object in memory that has already been allocated.

```cpp
alignas(MyClass) unsigned char storage[sizeof(MyClass)];

MyClass* object = new (storage) MyClass();
```

It is useful for fixed-size memory pools in firmware because memory can be statically allocated.

The destructor must be called manually:

```cpp
object->~MyClass();
```
