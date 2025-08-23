# Mockingbird

[![CMake on multiple platforms](https://github.com/muazsh/Mockingbird/actions/workflows/cmake-multi-platform.yml/badge.svg)](https://github.com/muazsh/Mockingbird/actions/workflows/cmake-multi-platform.yml)

:white_check_mark: All C++11 compilers and higher are supported.

Mockingbird is a mocking framework for c++, it is a simple yet a powerful framework, Mockingbird provides mocking virtual/non-virtual methods whether it is overloaded or not, const or not, template or not, and also it provides counting number of calls of a mocked method.

I am using here the term `method` to refer to a class member function and the term `function` to refer to a free function.   

For a class to be mocked Mockingbird creates a mocking class and then the developer should instantiate that mocking class and inject (stub) functions/lambdas instead of the methods which are to be mocked, so you need to create the mocking class once, then you can inject new behaviours on the fly, even several behaviours for the same method in the same test.

Mockingbird provides the following Macros to enable mocking any class:
-	`START_MOCK`: This must be the first macro call and it takes 2 arguments: the name of the generated mocking class and the name of the mocked class.
-	`FUNCTION`: This macro is to setup an injection functionality for any method, it takes the following arguments:
1.	Method name.
2.	Method return type.
3.	Method signature surrounded by parentheses and followed by `const` if the method is `const`.
4.	A pointer to a substitute function.
5.	The signature arguments names separated by commas.    
-	`FUNCTION_OVERLOADING`: This macro is to setup an injection functionality for the overloaded virtual methods, it takes the following arguments:
1.	Method name.
2.	Method return type.
3.	Method signature surrounded by parentheses and followed by `const` if the method is `const`.
4.	A pointer to a substitute function.
5.	A number for numbering the overloading methods.
6.	The signature arguments names separated by commas.
-	`FUNC`: This macro can be used instead of `FUNCTION` and `FUNCTION_OVERLOADING` with no need to number the overloaded methods, the injection and counter call Get methods of overloaded methods are distiguished via parameters names, so it simplifies the framework usage, but the methods parametes should not be more than 20 parameter.
-	`END_MOCK`: This must be the last macro call and it takes 1 argument: the name of the generated mocking class (optionally).

For each mocked method `Fx` in the mocking class there will be 3 corresponding methods:
1.	`Fx`: The method itself as a mock.
2.	`InjectFx`: This is to stub a substitute function.
3.	`GetFxCallCounter`: Returns the number of calls of the mocked method.

**Class Template Mock:**

Mockingbird supports mocking class templates, but the usage mechanism deviates a bit from mocking a no template class, where for mocking a no template class Mockingbird allows building one mocking class and then using that mock anywhere and simply injecting mocking methods, so one mock class and the behavior can be changed by injecting differnt mocking methods, however; for tempalte classes a new mock to be created for each new behavior and no injection is possible because injection is a run-time operation, the following macros are for class and function template:

-	`START_MOCK_TEMPLATE(MockingClass, MockedClass, ...)`
-	`FUNCTION_TEMPLATE(FuncName,ReturnType, Signature, Expression, .../*signature variables*/)`: `Expression` is the mock behavior.
-	`FUNCTION_TEMPLATE_OVERLOADING`

**Features**:
-	All C++11 compilers and higher should compile Mockingbird, compilation tested on: MSVC, GCC, Clang, ICC, ICX, ARM and DJGPP.
-	Mocking any virtual method (const and overloaded).
-	Hiding any non-virtual method (const and overloaded).
-	Counting the number of calls of mocked/hidden methods.
-	Supporting spying the original method.
-	Class template mock.     

**Setup**:

-	Just copy the ~140 LoC `Mockingbird.hpp` into your code and include it to start mocking your classes.

**Limitations**:

-	When injecting lambdas as a method substitute, that lambda is not allowed to have a capture.

**Usage**:
- **Simple example**:

Assume having the following legacy code and `Foo` class needs to be mocked:
```c++
struct MyStruct{
	int x, y;
};

class Foo{
public:
	virtual const MyStruct CreateMyStruct(int x, int y) { return MyStruct{ x,y }; }
}; 
```

Then a testing fixture should be written only once for the whole project:
```c++
const MyStruct CreateMyStructDummy(int x, int y) { return MyStruct{0,0}; }

START_MOCK(FooMock, Foo)
FUNCTION(CreateMyStruct, const MyStruct, (int x, int y), &CreateMyStructDummy, x, y)
END_MOCK(FooMock)
```

Then in the tests if a method needs some desired behaviour a substitute function or lambda (without capture) should be intorduced, for example:
```c++
const MyStruct CreateMyStructSubstitute(int x, int y) { return MyStruct{ x + 10, y + 10 }; }
```
and finally injecting the substitute and using `MockFoo` object as if it is of `Foo` like:
```c++
FooMock fooMock;
fooMock.InjectCreateMyStruct(CreateMyStructSubstitute); // Mocking function injection.
auto created = fooMock.CreateMyStruct(5,5);
EXPECT_EQ(15, created.x);
EXPECT_EQ(15, created.y);
```

As an alternative of injecting a function, a lambda can be injected as well in case it needs no capture like:
```c++
FooMock fooMock;
auto lambda = [](int x, int y)->const MyStruct{ return MyStruct{ x + 10, y + 10 }; };
fooMock.InjectCreateMyStruct(lambda); // Mocking behavior injection.
auto created = fooMock.CreateMyStruct(5,5);
EXPECT_EQ(15, created.x);
EXPECT_EQ(15, created.y);
```

- **Comprehensive example**:

This example includes overloading, verifying number of calls and spying the original behaviour.

Assume having the following legacy code and `Foo` class needs to be mocked:
``` c++
struct MyStruct{
	int x, y;
};

class Foo{
public:
	virtual void ResetMyStruct(MyStruct& myStruct) = 0;
	virtual const MyStruct CreateMyStruct(int x, int y) { return MyStruct{ x,y }; };
	virtual const MyStruct CreateMyStruct(int x) { return MyStruct{ x,x }; };
	virtual MyStruct MakeSpecialCopyMyStruct(const std::shared_ptr<MyStruct>& myStruct) const { return MyStruct{ myStruct->x, myStruct->y }; }
	virtual MyStruct MakeSpecialCopyMyStruct(const MyStruct& myStruct) const { return MyStruct{ myStruct.x, myStruct.y }; }
	virtual int GetTen() { return 10; }
	std::string GetString() { return "Original"; }
};

template<class T, class E>
class TemplatedFoo {
public:
	virtual T Sum(T x, E y) = 0;
	virtual T SumConst(T x, E y) const = 0;
	virtual T Sum(T x, E y, T z) = 0;
	~TemplatedFoo() {}
};
```
Assuming all methods except of `GetTen` are to be mocked, then a test fixture for the class `Foo` should be intorduced using **Mockingbird** macros and dummy versions of methods to be mocked, this fixture gets written only once for a project where `Foo` appears in tests as a mock:
```c++
void ResetMyStructDummy(MyStruct& myStruct) {}
const MyStruct CreateMyStructDummy(int x, int y) { return MyStruct(); }
const MyStruct CreateMyStructDummy1(int x) { return MyStruct(); }
MyStruct MakeSpecialCopyMyStructDummy(const std::shared_ptr<MyStruct>& myStruct) { return MyStruct(); } // This is a static method wihch cannot be const
MyStruct MakeSpecialCopyMyStructDummy1(const MyStruct& myStruct) { return MyStruct(); }
std::string GetStringDummy() { return ""; }

START_MOCK(FooMock, Foo)
FUNCTION(ResetMyStruct, void, (MyStruct& myStruct), &ResetMyStructDummy, myStruct)
FUNCTION(CreateMyStruct, const MyStruct, (int x, int y), &CreateMyStructDummy, x, y)
FUNCTION_OVERLOADING(CreateMyStruct, const MyStruct, (int x), &CreateMyStructDummy1, 1, x)
FUNCTION(MakeSpecialCopyMyStruct, MyStruct, (const std::shared_ptr<MyStruct>& myStruct)const, &MakeSpecialCopyMyStructDummy, myStruct)
FUNCTION_OVERLOADING(MakeSpecialCopyMyStruct, MyStruct, (const MyStruct& myStruct)const, &MakeSpecialCopyMyStructDummy1, 1, myStruct)
FUNCTION(GetString, std::string, (), &GetStringDummy)
END_MOCK(FooMock)
```
When calling one of `FUNCTION` macros note that when passing the signature arguments names at the end, they must be same as the names in the passed signature, so in case of ` CreateMyStruct ` `x,y` in the end are named same as in signature argument `(int x, int y)`.
Also note when mocking an overloaded method, the passed number N will show up in the method `GetFxNCallCounter` to distinguish what overloaded method calls number is wanted.

In the tests a substitute function should be introduced for each method wanted to get mocked:
``` c++
void ResetMyStructSubstitute(MyStruct& myStruct) { myStruct.x = 10; myStruct.y = 10; }
const MyStruct CreateMyStructSubstitute(int x, int y) { return MyStruct{ x + 10, y + 10 }; }
const MyStruct CreateMyStructSubstitute1(int x) { return MyStruct{ x + 5, x + 5 }; }
MyStruct MakeSpecialCopyMyStructSubstitute(const std::shared_ptr<MyStruct>& myStruct) { return MyStruct{ myStruct->x + 10, myStruct->y + 10 }; }
MyStruct MakeSpecialCopyMyStructSubstitute1(const MyStruct& myStruct) { return MyStruct{ myStruct.x + 15, myStruct.y + 15 }; }
std::string GetStringSubstitute() { return "Mock"; }
```
Finally substitutes need to be injected into an instance of `MockFoo` which can be used as if it is of a `Foo` class like:
``` c++
MyStruct myStruct{ 1, 1 };
FooMock fooMock;

fooMock.InjectMakeSpecialCopyMyStruct(MakeSpecialCopyMyStructSubstitute1); // Mocking methods injection.
auto specialCopy = fooMock.MakeSpecialCopyMyStruct(myStruct);
EXPECT_EQ(16, created.x);
EXPECT_EQ(16, created.y);
EXPECT_EQ(1, fooMock.GetMakeSpecialCopyMyStruct1CallCounter());
EXPECT_EQ(10, fooMock.GetTen()); // Spying.

fooMock.InjectGetString(GetStringSubstitute);
EXPECT_EQ("Mock", fooMock.GetString());
```

For class templates, as mentioned previously a new mock should be introduced for each new needed behavior (no injection is possible)(see tests):
```
START_MOCK_TEMPLATE(FooTemplatedMock, TemplatedFoo, typename T, typename E)
FUNCTION_TEMPLATE(Sum, T, (T x, E y), return x + y, x, y)
FUNCTION_TEMPLATE(SumConst, T, (T x, E y)const, return x + y, x, y)
FUNCTION_TEMPLATE_OVERLOADING(Sum, T, (T x, E y, T z), return x + y + z, 1, x, y, z)
END_MOCK(FooTemplatedMock)
```
It is important to note that the number 1 in `GetMakeSpecialCopyMyStruct1CallCounter` is the number passed in the fixture `FUNCTION_OVERLOADING(CreateMyStruct, const MyStruct, (int x), &CreateMyStructDummy1, 1, x)` before `x` parameter in this example. 

**Note**: 
-	For a mocked method if a substitute is not injected the default behavior will be the behaviour of the dummy function in the fixture.
-	When hiding a non-virtual method polymorphism won't work anymore (of course because the method is not virtual, see Hide test in test.cpp).





