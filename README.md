# Mockingbird

[![CMake on multiple platforms](https://github.com/muazsh/Mockingbird/actions/workflows/cmake-multi-platform.yml/badge.svg)](https://github.com/muazsh/Mockingbird/actions/workflows/cmake-multi-platform.yml)

A **lightweight, simple and powerful mocking framework for C++11 and above**.

Mockingbird equipes the created mock with mechanism to inject new behaviours in runtime instead of the target class methods. 

---

## ✨ Features
- ✅ Cross-platform, works with **all C++11+ compilers** (this github pipeline runs tests against MSVC, GCC, Clang).
- 🎯 Mock any **virtual** method and hide **non-virtual** ones.
- 🔢 Call counters for every mocked method.
- 🔍 Spying on original implementations supported.
- 📦 Header-only (`Mockingbird.hpp`, ~140 LoC).
- 🧩 Full **class template mocking** support.
- ✅ Can be integrated with any testing framework like Catch2, gtest and Microsoft Unit Testing Framework.
- 🎯 No macros in tests, macros are used only when defining the mock.

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

### Core Macros
| Macro | Purpose | Notes |
|-------|----------|-------|
| `START_MOCK` | Begin mock definition | Must be first macro |
| `FUNCTION` | Mock a single method | Works with non-overloaded methods |
| `FUNCTION_OVERLOADING` | Mock an overloaded method | Requires overload number |
| `FUNC` | Shortcut for `FUNCTION` & `FUNCTION_OVERLOADING` | Limited: ≤20 params, no rvalue refs |
| `END_MOCK` | End mock definition | Must be last macro |

#### Core Macros Parameters

-	`START_MOCK(MockClass, TargetClass)`: 
    1.  MockClass: Mock class name.
    2.  TargetClass: Target class name.
-	`FUNCTION(Name, ReturnType, Signature, SubFn, Args...)`:
    1.	Name: The Method name.
    2.	ReturnType: Method return type.
    3.	Signature: Method signature surrounded by parentheses and followed by `const` if the method is `const`.
    4.	SubFn: A pointer to a substitute function.
    5.	Args: The signature parameters names separated by commas.    
-	`FUNCTION_OVERLOADING(Name, ReturnType, Signature, SubFn, Disc, Args...)`: All are same as `FUNCTION` macro paramerters, in addition to:
    1.	Disc: A dicriminator, to distiguish between overloaded methods.
-	`FUNC(Name, ReturnType, Signature, SubFn, Args...)`: Same as `FUNCTION` macro paramerters.

-	`END_MOCK(MockClass)`: 
    1.  MockClass: Mock class name (optional parameter).


### Template Macros
| Macro | Purpose |
|-------|----------|
| `START_MOCK_TEMPLATE(MockClass, TargetClass, Types...)` | Begin template mock |
| `FUNCTION_TEMPLATE(FuncName,ReturnType, Signature, Expression, Args...)` | Mock template method |
| `FUNCTION_TEMPLATE_CONST(FuncName,ReturnType, Signature, Expression, Args...)` | Mock const template method |
| `FUNCTION_TEMPLATE_OVERLOADING(FuncName,ReturnType, Signature, Expression, Disc, Args...)` | Overloaded template mock |
| `FUNCTION_TEMPLATE_OVERLOADING_CONST(FuncName,ReturnType, Signature, Expression, Disc, Args...)` | Const overloaded template mock |

#### Template Macros Parameters

All parameters are same as core macros paramerters, except:

- Types: In `START_MOCK_TEMPLATE` this variable parameters are the template types.
- Expression: In `FUNCTION_TEMPLATE*` macros, instead of `SubFn` in core macros, this expression is the default function template behaviour, 


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

  In these cases, you must use `FUNCTION` or `FUNCTION_OVERLOADING` instead.

- If no substitute is injected, the **dummy function** (or the **Expression** in template case) in the fixture is used.
- Non-virtual methods, when hidden, **lose polymorphism**.
- Injected lambdas **cannot have captures**.
- The [Mockingbird extension](https://marketplace.visualstudio.com/items?itemName=MouazChamieh.mockingbirdextension) can auto-generate mocks using AI.

---

## 📑 Usage Examples

### 1️⃣ Basic Example
```cpp
struct MyStruct { int x, y; };

class Foo {
public:
    virtual const MyStruct CreateMyStruct(int x, int y) { return {x, y}; }
};

const MyStruct CreateMyStructDummy(int x, int y) { return {0, 0}; }

START_MOCK(FooMock, Foo)
    FUNCTION(CreateMyStruct, const MyStruct, (int x, int y), &CreateMyStructDummy, x, y)
END_MOCK(FooMock)
```

In tests:
```cpp
FooMock fooMock;
fooMock.InjectCreateMyStruct([](int x, int y){ return MyStruct{x+10, y+10}; });

auto result = fooMock.CreateMyStruct(5, 5);
EXPECT_EQ(15, result.x);
EXPECT_EQ(15, result.y);
```

---

### 2️⃣ Comprehensive Example (Overloading, Call Counters, Spying)

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
void ResetMyStructDefault(MyStruct& myStruct) {}
const MyStruct CreateMyStructDefault(int x, int y) { return MyStruct{ 0,0 }; }
const MyStruct CreateMyStructDefault1(int x) { return MyStruct{ 0,0 }; }
MyStruct MakeSpecialCopyMyStructDefault(const std::shared_ptr<MyStruct>& myStruct) { return MyStruct{ 0,0 }; }
MyStruct MakeSpecialCopyMyStructDefault1(const MyStruct& myStruct) { return MyStruct{ 0,0 }; }
MyStruct MakeSpecialCopyMyStructDefault2(MyStruct&& myStruct) { return MyStruct{ 0,0 }; }
std::string GetStringDefault() { return ""; }

START_MOCK(FooMock, Foo)
    FUNC(ResetMyStruct, void, (MyStruct& myStruct), &ResetMyStructDefault, myStruct)
    FUNC(CreateMyStruct, const MyStruct, (int x, int y), &CreateMyStructDefault, x, y)
    FUNC(CreateMyStruct, const MyStruct, (int x), &CreateMyStructDefault1, x)
    FUNC(MakeSpecialCopyMyStruct, MyStruct, (const std::shared_ptr<MyStruct>& myStructPtr)const, &MakeSpecialCopyMyStructDefault, myStructPtr)
    FUNC(MakeSpecialCopyMyStruct, MyStruct, (const MyStruct& myStruct)const, &MakeSpecialCopyMyStructDefault1, myStruct)
    FUNCTION(MakeSpecialCopyMyStruct, MyStruct, (MyStruct&& myStructRLR)const, &MakeSpecialCopyMyStructDefault2, std::move(myStructRLR))
    FUNC(GetString, std::string, (), &GetStringDefault)
END_MOCK(FooMock)
```

**In Tests**:
```cpp
FooMock fooMock;
fooMock.InjectCreateMyStruct([](int x, int y){ return MyStruct{x+10, y+10}; });

auto myStruct = fooMock.CreateMyStruct(5,5);
EXPECT_EQ(15, myStruct.x);
EXPECT_EQ(15, myStruct.y);
EXPECT_EQ(1, fooMock.GetCreateMyStructCallCounter());
EXPECT_EQ(10, fooMock.GetTen()); // Spy real implementation since not presented in the mock definition.
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
};

START_MOCK_TEMPLATE(FooTemplatedMock, TemplatedFoo, T, E)
    FUNCTION_TEMPLATE(Sum, T, (T x, E y), return 0, x, y)
    FUNCTION_TEMPLATE_OVERLOADING(Sum, T, (T x, E y, T z), return 0, 1, x, y, z)
    FUNCTION_TEMPLATE_CONST(SumConst, T, (T x, E y), return 0, x, y)
    FUNCTION_TEMPLATE_OVERLOADING_CONST(SumConst, T, (T x, E y, int&& z), return 0, 1, x, y, std::move(z))
END_MOCK(FooTemplatedMock)
```

---

## ⚠️ Limitations
- Injected lambdas **cannot have captures**.
- For methods with >20 parameters or rvalue refs, use `FUNCTION` / `FUNCTION_OVERLOADING` instead of `FUNC`.

---

## 🔧 Tooling
- [Visual Studio Extension (auto-mock generator)](https://marketplace.visualstudio.com/items?itemName=MouazChamieh.mockingbirdextension)

---

## 🤝 Contributing
Contributions, issues, and feature requests are welcome!






