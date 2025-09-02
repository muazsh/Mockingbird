# Mockingbird

[![CMake on multiple platforms](https://github.com/muazsh/Mockingbird/actions/workflows/cmake-multi-platform.yml/badge.svg)](https://github.com/muazsh/Mockingbird/actions/workflows/cmake-multi-platform.yml)

A **lightweight, simple and powerful mocking framework for C++11 and above**.

Mockingbird equipes the created mock with mechanism to inject new behaviours in runtime instead of the target class methods. 

---

## Features
-  Cross-platform, works with **all C++11+ compilers** (this github pipeline runs tests against MSVC, GCC, Clang).
-  Mock any **virtual** method and hide **non-virtual** ones.
-  Call counters for every mocked method.
-  Spying on original implementations supported.
-  Header-only (`Mockingbird.hpp`, ~130 LoC).
-  Full **class template mocking** support.
-  Can be integrated with any testing framework like Catch2, gtest and Microsoft Unit Testing Framework.
-  No macros in tests, macros are used only to define the mock class.

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
| `FUNCTION` | Mock a `non-const` method | Works when no overloading yet |
| `FUNCTION_CONST` | Mock a `const` method | Works when no overloading yet |
| `FUNCTION_OVERLOAD` | Mock a `non-const` overloaded method | Requires overload discriminator |
| `FUNCTION_OVERLOAD_CONST` | Mock a `const` overloaded method | Requires overload discriminator |
| `FUNC` | Shortcut for `FUNCTION`, `FUNCTION_CONST`, `FUNCTION_OVERLOAD` and `FUNCTION_OVERLOAD_CONST` | Limited: ≤20 params, no rvalue refs params |
| `END_MOCK` | End mock definition | Must be last macro |

#### Macros Parameters

-	`START_MOCK(MockClass, TargetClass)`: 
    1.  MockClass: Mock class name.
    2.  TargetClass: Target class name.
-	`START_MOCK_TEMPLATE(MockClass, TargetClass, Types...)`: Same as `START_MOCK` parameters, in addition to: 
    1.  Types: Variable arguments represent the template types.
-	`FUNCTION(Name, ReturnType, Signature, Expression, Args...)`:
    1.	Name: The Method name.
    2.	ReturnType: Method return type.
    3.	Signature: Method signature surrounded by parentheses.
    4.	Expression: The method default behaviour, should contain no commas.
    5.	Args: The signature parameters names separated by commas.    
-	`FUNCTION_CONST(Name, ReturnType, Signature, Expression, Args...)`: Same as `FUNCTION` parameters.
-	`FUNCTION_OVERLOAD(Name, ReturnType, Signature, Expression, Disc, Args...)`: Same as `FUNCTION` paramerters, in addition to:
    1.	Disc: A dicriminator, to distiguish between overloaded methods.
-	`FUNCTION_OVERLOAD_CONST(Name, ReturnType, Signature, Expression, Disc, Args...)`: Same as `FUNCTION_OVERLOAD` paramerters.
-	`FUNC(Name, ReturnType, Signature, SubFn, Args...)`: Same as `FUNCTION` paramerters except:
    1. SubFn: Instead of default `Expression` in other variations, this is a function pointer where the function is the default behaviour.

-	`END_MOCK(MockClass)`: 
    1.  MockClass: Mock class name (optional parameter).

### Generated Methods
For each mocked method `Fx`, Mockingbird generates:
1. `Fx(...)` → The mock method.
2. `InjectFx(...)` → Inject substitute function/lambda.
3. `GetFxCallCounter()` → Retrieve number of calls.

---

## 📌 Important Notes
- `FUNC` **cannot** be used when:
  - The mocked method has **more than 20 parameters**, or
  - The mocked method has an **rvalue reference parameter** (`T&&`).

  In those 2 cases, `FUNCTION`, `FUNCTION_CONST`, `FUNCTION_OVERLOAD` or `FUNCTION_OVERLOAD_CONST` to be used instead.

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
fooMock.InjectCreateMyStruct([](int x, int y){ return MyStruct{x+10, y+10}; });

auto result = fooMock.CreateMyStruct(5, 5);
assert(15 == result.x);
assert(15 == result.y);
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
    FUNCTION(ResetMyStruct, void, (MyStruct& myStruct), return, myStruct)
    FUNCTION(CreateMyStruct, const MyStruct, (int x, int y), return MyStruct{}, x, y)
    FUNCTION_OVERLOAD(CreateMyStruct, const MyStruct, (int x), return MyStruct{}, OnlyX, x)
    FUNCTION_CONST(MakeSpecialCopyMyStruct, MyStruct, (const MyStruct& myStruct), return MyStruct{}, myStruct)
    FUNCTION_OVERLOAD_CONST(MakeSpecialCopyMyStruct, MyStruct, (const std::shared_ptr<MyStruct>& myStructPtr), return MyStruct{}, Shared, myStructPtr)
    FUNCTION_OVERLOAD_CONST(MakeSpecialCopyMyStruct, MyStruct, (MyStruct&& myStructRLR), return MyStruct{}, RVRef, std::move(myStructRLR))
    FUNCTION(GetString, std::string, (), return "")
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

### 3️⃣ FUNC Simplified Macro Example

**Hoo Class**
```cpp
class Hoo {
public:
	virtual void ResetMyStruct(MyStruct& myStruct) = 0;
	virtual const MyStruct CreateMyStruct(int x, int y) { return MyStruct{ x,y }; };
	virtual const MyStruct CreateMyStruct(int x) const { return MyStruct{ x,x }; };
	virtual ~Hoo() {}
};
```

**Mock Definition**:
```cpp

void ResetMyStructDefault(MyStruct& myStruct) {}
const MyStruct CreateMyStructDefault1(int x, int y) { return MyStruct{ 0,0 }; }
const MyStruct CreateMyStructDefault2(int x) { return MyStruct{ 0,0 }; }

START_MOCK(HooMock, Hoo)
    FUNC(ResetMyStruct, void, (MyStruct& myStruct), &ResetMyStructDefault, myStruct)
    FUNC(CreateMyStruct, const MyStruct, (int x, int y), &CreateMyStructDefault1, x, y)
    FUNC(CreateMyStruct, const MyStruct, (int x) const, &CreateMyStructDefault2, x)
END_MOCK(HooMock)
```

**In Tests**:
```cpp
HooMock hooMock;

hooMock.InjectCreateMyStruct_xy([](int x, int y) -> const MyStruct { return MyStruct{x+10, y+10}; });
auto myStruct1 = hooMock.CreateMyStruct(5,0);
assert(15 == myStruct1.x);
assert(10 == myStruct1.y);
assert(1 == hooMock.GetCreateMyStructCallCounter_xy());

hooMock.InjectCreateMyStruct_x([](int x) -> const MyStruct { return MyStruct{x+100, x+100}; });
auto myStruct2 = hooMock.CreateMyStruct(5);
assert(105 == myStruct2.x);
assert(105 == myStruct2.y);
assert(1 == hooMock.GetCreateMyStructCallCounter_x());
```
---

### 4️⃣ Template Example

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
    FUNCTION(Sum, T, (T x, E y), return T{}, x, y)
    FUNCTION_OVERLOAD(Sum, T, (T x, E y, T z), return T{}, OverloadWithZ, x, y, z)
    FUNCTION_CONST(SumConst, T, (T x, E y), return T{}, x, y)
    FUNCTION_OVERLOAD_CONST(SumConst, T, (T x, E y, int&& z), return T{}, RvalRef, x, y, std::move(z))
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






