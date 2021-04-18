# Mockingbird

:white_check_mark: All compilers with c++11 and higher are supported: [Choose a compiler to build](https://godbolt.org/z/TKbG44Pra)

Mockingbird is a mocking framework for c++, it is a simple yet a powerful framework and it provides mocking overloaded methods and counting number of calls of a mocked method.

I am using here the word `method` to refer to a class member function and the word `function` to refer to a free function.   

For a class to be mocked Mockingbird creates a mocking class and then the developer should instantiate that mocking class and inject (stub) functions instead of the methods wanted to be mocked.

Mockingbird provides 6 Macros to mock any class with virtual methods:
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
-	`END_MOCK`: This must be the last macro call and it takes 1 argument: the name of the generated mocking class (optionally).
For each mocked method `Fx` in the mocking class there will be 3 corresponding methods:
1.	`Fx`: The method itself.
2.	`InjectFx`: This is to stub a substitute function.
3.	`GetFxCallCounter`: Returns the number of calls of the mocked method.

Here is a comprehensive usage example:
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
}; 
```
Assuming all methods except of `GetTen` are to be mocked, then a test fixture for the class `Foo` should be intorduced using **Mockingbird** macros and dummy versions of methods to be mocked, this fixture gets written only once for a project where `Foo` appears in tests as a mock:
```c++
void ResetMyStructDummy(MyStruct& myStruct) {}
const MyStruct CreateMyStructDummy(int x, int y) { return MyStruct(); }
const MyStruct CreateMyStructDummy2(int x) { return MyStruct(); }
MyStruct MakeSpecialCopyMyStructDummy(const std::shared_ptr<MyStruct>& myStruct) { return MyStruct(); } // This is a static method wihch cannot be const
MyStruct MakeSpecialCopyMyStructDummy2(const MyStruct& myStruct) { return MyStruct(); }

START_MOCK(FooMock, Foo)
FUNCTION(ResetMyStruct, void, (MyStruct& myStruct), &ResetMyStructDummy, myStruct)
FUNCTION(CreateMyStruct, const MyStruct, (int x, int y), &CreateMyStructDummy, x, y)
FUNCTION_OVERLOADING(CreateMyStruct, const MyStruct, (int x), &CreateMyStructDummy2, 1, x)
CONST_FUNCTION(MakeSpecialCopyMyStruct, MyStruct, (const std::shared_ptr<MyStruct>& myStruct), &MakeSpecialCopyMyStructDummy, myStruct)
CONST_FUNCTION_OVERLOADING(MakeSpecialCopyMyStruct, MyStruct, (const MyStruct& myStruct), &MakeSpecialCopyMyStructDummy2, 1, myStruct)
END_MOCK(FooMock)
```
When calling one of `FUNCTION` macros note that when passing the signature arguments names at the end, they must be same as the names in the passed signature, so in case of ` CreateMyStruct ` `x,y` in the end are named same as in signature argument `(int x, int y)`.
Also note when mocking an overloaded method, the passed number N will show up in the method `GetFxNCallCounter` to distinguish what overloaded method calls number is wanted.

In the tests a substitute function should be introduced for each method wanted to get mocked:
``` c++
void ResetMyStructSubstitute(MyStruct& myStruct) { myStruct.x = 10; myStruct.y = 10; }
const MyStruct CreateMyStructSubstitute(int x, int y) { return MyStruct{ x + 10, y + 10 }; }
const MyStruct CreateMyStructSubstitute2(int x) { return MyStruct{ x + 5, x + 5 }; }
MyStruct MakeSpecialCopyMyStructSubstitute(const std::shared_ptr<MyStruct>& myStruct) { return MyStruct{ myStruct->x + 10, myStruct->y + 10 }; }
MyStruct MakeSpecialCopyMyStructSubstitute2(const MyStruct& myStruct) { return MyStruct{ myStruct.x + 15, myStruct.y + 15 }; }
```
Finally substitutes need to be injected into an instance of `MockFoo` which can be used as if it is of a `Foo` class like:
``` c++
MyStruct myStruct{ 1, 1 };
FooMock fooMock;
fooMock.InjectMakeSpecialCopyMyStruct(MakeSpecialCopyMyStructSubstitute); // Mocking methods injection.
auto specialCopy = fooMock.MakeSpecialCopyMyStruct(std::make_shared<MyStruct>(myStruct));
```

**Note**: For a mocked method if a substitute is not injected the default behavior will be the behaviour of the dummy function in the fixture.


