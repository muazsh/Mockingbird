# Mockingbird

[![CMake on multiple platforms](https://github.com/muazsh/Mockingbird/actions/workflows/cmake-multi-platform.yml/badge.svg)](https://github.com/muazsh/Mockingbird/actions/workflows/cmake-multi-platform.yml)

A **lightweight, simple and powerful mocking framework for C++11 and above**.

Mockingbird equipes the created mock with mechanism to inject new behaviors in runtime instead of the target class methods. 

---

## Features
-  **Header-only** (`Mockingbird.hpp`, ~100 LoC).
-  Cross-platform, works with **all C++11+ compilers** (this github pipeline runs tests against MSVC, GCC, Clang).
-  Mock **virtual** methods and hide **non-virtual** ones.
-  Mock **const** and **overloaded** methods.
-  Set **different behaviors** for the first **n** mock calls.
-  Call **counters** for every mocked method.
-  **Spying** on original implementations supported.
-  **Multiple inheritance** mock.
-  Full **class template mock** support.
-  Can be **integrated** with any testing framework like Catch2, gtest and Microsoft Unit Testing Framework.
-  **No macros** in tests, macros are used only to define the mock class.

---

## 🚀 Quick Start

### Installation
Just copy [`Mockingbird.hpp`](./Mockingbird.hpp) into your project and include it:

```cpp
#include "Mockingbird.hpp"
```

No build system integration required.

When building using MSVC, the standard conforming preprocessor should be incorporated, by setting **/Zc:preprocessor** compiler flag.

---

## 📘 API Reference

### Macros
| Macro | Purpose | Notes |
|-------|----------|-------|
| `START_MOCK` | Begin mock definition | Must be first macro |
| `START_MOCK_TEMPLATE` | Begin class template mock definition | Must be first macro in case of class template mock |
| `FUNCTION` | Mock a method | Works when no overloading yet |
| `FUNCTION_OVERLOAD` | Mock an overloaded method | Requires overload discriminator |
| `END_MOCK` | End mock definition | Must be last macro |

#### Macros Parameters

-	`START_MOCK(MockClass, TargetClasses...)`: 
    1.  MockClass: Mock class name.
    2.  TargetClasses: Target classes names (might be many in case of multiple inheritance).
-	`START_MOCK_TEMPLATE(MockClass, TargetClass, Types...)`: Same as `START_MOCK` parameters, in addition to: 
    1.  Types: Variable arguments represent the template types.
-	`FUNCTION(Name, ReturnType, Signature, Const, Override, Expression, Args...)`:
    1.	Name: The Method name.
    2.	ReturnType: Method return type.
    3.	Signature: Method signature surrounded by parentheses.
    4.  Const: whether the function is `const`, should be either `const` or empty.
    5.  Override: whether the function is an `override`, should be either `override` or empty.
    6.	Expression: The method default behaviour, should contain no commas.
    7.	Args: The signature parameters names separated by commas.    
-	`FUNCTION_OVERLOAD(Name, ReturnType, Signature, Expression, Disc, Args...)`: Same as `FUNCTION` paramerters, in addition to:
    1.	Disc: A dicriminator, to distiguish between overloaded methods.
-	`END_MOCK(MockClass)`: 
    1.  MockClass: Mock class name (optional parameter).

### Generated Methods
For each mocked method `Fx`, Mockingbird generates:
1. `Fx(...)` → The override/hide mock method.
2. `InjectFx(...)` → Inject substitute function/lambda.
3. `InjectFxCallOnce` → Inject substitute function/lambda to be called once, calling it `n` times causes calling these injections for the first `n` `Fx` calls in the injection order. 
4. `GetFxCallCounter()` → Retrieve number of calls.

---

## 📌 Important Notes
- If no substitute is injected, the **Expression** or **SubFn** in the fixture is the default behaviour.
- Non-virtual methods, when hidden, **lose polymorphism**.
- Injected lambdas **cannot have captures**.
- The [Mockingbird extension](https://marketplace.visualstudio.com/items?itemName=MouazChamieh.mockingbirdextension) can auto-generate mocks using AI.

---

## 📑 Usage Examples

### 1️⃣ Basic Example
```cpp
struct MyStruct 
{ 
    int x = 0; 
    int y = 0; 
};

class Foo {
public:
    virtual const MyStruct CreateMyStruct(int x, int y) { return {x, y}; }
};

START_MOCK(FooMock, Foo)
    FUNCTION(CreateMyStruct, const MyStruct, (int x, int y), return MyStruct{}, x, y)
END_MOCK(FooMock)
```

**In Tests**:
```cpp
FooMock fooMock;

auto beforeInjection = fooMock.CreateMyStruct(5, 5); // returns default MyStruct{}
assert(0 == beforeInjection.x);
assert(0 == beforeInjection.y);

fooMock.InjectCreateMyStructCallOnce([](int x, int y) -> const MyStruct {return MyStruct{ 10,10 }; }); // injection for the first call
fooMock.InjectCreateMyStructCallOnce([](int x, int y) -> const MyStruct {return MyStruct{ 20,20 }; }); // injection for the second call
fooMock.InjectCreateMyStruct([](int x, int y) -> const MyStruct {return MyStruct{ x + 10, y + 10 }; }); // injection for calls after second

auto firstMyStruct = fooMock.CreateMyStruct(5, 5);
assert(10 == firstMyStruct.x);
assert(10 == firstMyStruct.y);

auto secondMyStruct = fooMock.CreateMyStruct(5, 5);
assert(20 == secondMyStruct.x);
assert(20 == secondMyStruct.y);

auto thirdMyStruct = fooMock.CreateMyStruct(5, 5);
assert(15 == thirdMyStruct.x);
assert(15 == thirdMyStruct.y);
```

---

### 2️⃣ Comprehensive FUNCTION* Macro Example

**Foo Class**
```cpp
class Foo {
public:
	virtual void ResetMyStruct(MyStruct& myStruct) = 0;
	virtual const MyStruct CreateMyStruct(int x, int y) { return MyStruct{ x,y }; };
	virtual const MyStruct CreateMyStruct(int x) { return MyStruct{ x,x }; };
	virtual MyStruct MakeSpecialCopyMyStruct(const std::shared_ptr<MyStruct>& myStruct) const { return MyStruct{ myStruct->x, myStruct->y }; }
	virtual MyStruct MakeSpecialCopyMyStruct(const MyStruct& myStruct) const { return MyStruct{ myStruct.x, myStruct.y }; }
	virtual MyStruct MakeSpecialCopyMyStruct(MyStruct&& myStruct) const { return MyStruct{ myStruct.x, myStruct.y }; }
	virtual int GetTen() { return 10; }
	std::string GetString() { return "Original"; }
	virtual ~Foo() {}
};
```

**Mock Definition**:
```cpp
START_MOCK(FooMock, Foo)
    FUNCTION(ResetMyStruct, void, (MyStruct& myStruct),, override, return, myStruct)
    FUNCTION(CreateMyStruct, const MyStruct, (int x, int y),, override, return MyStruct{}, x, y)
    FUNCTION_OVERLOAD(CreateMyStruct, const MyStruct, (int x),, override, return MyStruct{}, OnlyX, x)
    FUNCTION(MakeSpecialCopyMyStruct, MyStruct, (const MyStruct& myStruct), const, override, return MyStruct{}, myStruct)
    FUNCTION_OVERLOAD(MakeSpecialCopyMyStruct, MyStruct, (const std::shared_ptr<MyStruct>& myStructPtr), const, override, return MyStruct{}, Shared, myStructPtr)
    FUNCTION_OVERLOAD(MakeSpecialCopyMyStruct, MyStruct, (MyStruct&& myStructRLR), const, override, return MyStruct{}, RVRef, std::move(myStructRLR))
    FUNCTION(GetString, std::string, (), , , return "")
END_MOCK(FooMock)
```

**In Tests**:
```cpp
FooMock fooMock;
fooMock.InjectCreateMyStruct([](int x, int y){ return MyStruct{x+10, y+10}; });

auto myStruct = fooMock.CreateMyStruct(5,5);
assert(15 == myStruct.x);
assert(15 == myStruct.y);
assert(1 == fooMock.GetCreateMyStructCallCounter());
assert(10 == fooMock.GetTen()); // Spy real implementation since not presented in the mock definition.
```
---

### 3️⃣ Template Example

```cpp
template<class T, class E>
class TemplatedFoo {
public:
	virtual T Sum(T x, E y) = 0;
	virtual T Sum(T x, E y, T z) = 0;
	virtual T SumConst(T x, E y) const = 0;
	virtual T SumConst(T x, E y, int&& z) const = 0;
	virtual ~TemplatedFoo() {}
};

START_MOCK_TEMPLATE(FooTemplatedMock, TemplatedFoo, T, E)
    FUNCTION(Sum, T, (T x, E y), , override, return T{}, x, y)
    FUNCTION_OVERLOAD(Sum, T, (T x, E y, T z), , override, return T{}, OverloadWithZ, x, y, z)
    FUNCTION(SumConst, T, (T x, E y), const, override, return T{}, x, y)
    FUNCTION_OVERLOAD(SumConst, T, (T x, E y, int&& z), const, override, return T{}, RvalRef, x, y, std::move(z))
END_MOCK(FooTemplatedMock)
```

**In Tests**:
```cpp
double sumMock1(double x, float y, double z) {
	return x + y + z;
}

FooTemplatedMock<double, float> fooTemplatedMock;
fooTemplatedMock.InjectSum(&sumMock1);

auto sumMock2 = [](double x, float y) {return x + y; };
fooTemplatedMock.InjectSum(sumMock2);
fooTemplatedMock.InjectSumConst(sumMock2);

auto sumMock3 = [](double x, float y, int&& z) {return x + y + z; };
fooTemplatedMock.InjectSumConst(sumMock3);

assert(2 == fooTemplatedMock.Sum(1,1)); 
assert(4 == fooTemplatedMock.SumConst(2, 2)); 
assert(3 == fooTemplatedMock.Sum(1, 1, 1));
assert(6 == fooTemplatedMock.SumConst(1, 2, std::move(3)));
```
---

## ⚠️ Limitations
- Injected lambdas **cannot have captures**.

---

## 🔧 Tooling
- [Visual Studio Extension (auto-mock generator)](https://marketplace.visualstudio.com/items?itemName=MouazChamieh.mockingbirdextension)

---

## 🤝 Contributing
Contributions, issues, and feature requests are welcome!
