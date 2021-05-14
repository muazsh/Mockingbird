# Mockingbird

All compilers with c++11 and higher are supported.

Mockingbird is a mocking framework for c++, it is a simple yet a powerful framework and it provides mocking overloaded methods and counting number of calls of a mocked method.

I am using here the word `method` to refer to a class member function and the word `function` to refer to a free function.   

For a class to be mocked Mockingbird creates a mocking class and then the developer should instantiate that mocking class and inject (stub) functions instead of the methods wanted to be mocked.

Mockingbird provides the following Macros to enable mocking any class:
-	` START_MOCK`: This must be the first macro call and it takes 2 arguments: the name of the generated mocking class and the name of the mocked class.
-	`FUNCTION`: This macro is to setup an injection functionality for any `non-const` virtual method, it takes the following arguments:
1.	Method name.
2.	Method return type.
3.	Method signature surrounded by parentheses.
4.	A pointer to the substitute function.
5.	The signature arguments names separated by commas. 
-	`CONST_FUNCTION`: This macro is to setup an injection functionality for any `const` virtual method, it takes same arguments as `FUNCTION` macro.    
-	`FUNCTION_OVERLOADING`: This macro is to setup an injection functionality for the overloaded virtual methods, it takes the following arguments:
1.	Method name.
2.	Method return type.
3.	Method signature surrounded by parentheses.
4.	A pointer to the substitute function.
5.	A number for numbering the overloading methods.
6.	The signature arguments names separated by commas. 
-	`CONST_FUNCTION_OVERLOADING `: This macro is to setup an injection functionality for the `const` overloaded virtual methods, it takes same arguments as `FUNCTION_OVERLOADING` macro.
-	`HIDE`: Same as `FUNCTION` but for mocking non-virtual methods,
-	`CONST_HIDE`: Same as `CONST_FUNCTION` but for mocking non-virtual methods.
-	`HIDE_OVERLOADING`: Same as `FUNCTION_OVERLOADING` but for mocking non-virtual methods.
-	`CONST_HIDE_OVERLOADING`: Same as `CONST_FUNCTION_OVERLOADING` but for mocking non-virtual methods.
-	`END_MOCK`: This must be the last macro call and it takes 1 argument: the name of the generated mocking class (optionally).

For each mocked method `Fx` in the mocking class there will be 3 corresponding methods:
1.	`Fx`: The method itself.
2.	`InjectFx`: This is to stub a substitute function.
3.	`GetFxCallCounter`: Returns the number of calls of the mocked method.

**Features**:
-	All compilers with c++11 and hight should compile Mockingbird, compilation tested on: MSVC, GCC, Clang, ICC, ICX, ARM and DJGPP.
-	Mocking any virtual method (const and overloaded).
-	Hiding any non-virtual method (const and overloaded).
-	Counting number of calls of mocked/hidden methods.
-	Supporting spying the original method.

**Limitations**:
-	Mocked class must have a default constructor.


Here are two examples:
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

Then in the tests if a method needs some desired behaviour a substitute should be intorduced, for example:
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
CONST_FUNCTION(MakeSpecialCopyMyStruct, MyStruct, (const std::shared_ptr<MyStruct>& myStruct), &MakeSpecialCopyMyStructDummy, myStruct)
CONST_FUNCTION_OVERLOADING(MakeSpecialCopyMyStruct, MyStruct, (const MyStruct& myStruct), &MakeSpecialCopyMyStructDummy1, 1, myStruct)
HIDE(GetString, std::string, (), &GetStringDummy)
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
It is important to note that the number 1 in `GetMakeSpecialCopyMyStruct1CallCounter` is the number passed in the fixture `FUNCTION_OVERLOADING(CreateMyStruct, const MyStruct, (int x), &CreateMyStructDummy1, 1, x)` before `x` parameter. 

**Note**: 
-	For a mocked method if a substitute is not injected the default behavior will be the behaviour of the dummy function in the fixture.
-	When hiding a non-virtual method polymorphism won't work anymore (of course because the method is not virtual, see Hide test in test.cpp).





