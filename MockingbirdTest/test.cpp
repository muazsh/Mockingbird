#include "pch.h"

#pragma region Legacy Code Assumption
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

MyStruct GetMyStructFoo(const std::unique_ptr<Foo>& foo, int x, int y){ 
	auto myStruct = foo->CreateMyStruct(x,y); 
	foo->ResetMyStruct(myStruct);
	return foo->MakeSpecialCopyMyStruct(std::make_shared<MyStruct>(myStruct));
}

#pragma endregion

#pragma region Mocking Fixture
void ResetMyStructSubstitute(MyStruct& myStruct) { myStruct.x = 10; myStruct.y = 10; }
const MyStruct CreateMyStructSubstitute(int x, int y) { return MyStruct{ x + 10, y + 10 }; }
const MyStruct CreateMyStructSubstitute2(int x) { return MyStruct{ x + 5, x + 5 }; }
MyStruct MakeSpecialCopyMyStructSubstitute(const std::shared_ptr<MyStruct>& myStruct) { return MyStruct{ myStruct->x + 10, myStruct->y + 10 }; } // This is a static method wihch cannot be const
MyStruct MakeSpecialCopyMyStructSubstitute2(const MyStruct& myStruct) { return MyStruct{ myStruct.x + 15, myStruct.y + 15 }; }

START_MOCK(FooMock, Foo)
FUNCTION(ResetMyStruct, void, (MyStruct& myStruct), &ResetMyStructSubstitute, myStruct)
FUNCTION(CreateMyStruct, const MyStruct, (int x, int y), &CreateMyStructSubstitute, x, y)
FUNCTION_OVERLOADING(CreateMyStruct, const MyStruct, (int x), &CreateMyStructSubstitute2, 1, x)
CONST_FUNCTION(MakeSpecialCopyMyStruct, MyStruct, (const std::shared_ptr<MyStruct>& myStruct), &MakeSpecialCopyMyStructSubstitute, myStruct)
CONST_FUNCTION_OVERLOADING(MakeSpecialCopyMyStruct, MyStruct, (const MyStruct& myStruct), &MakeSpecialCopyMyStructSubstitute2, 1, myStruct)
END_MOCK(FooMock)
#pragma endregion

TEST(Mockingbird, VoidReturnReferenceSignature){
	MyStruct myStruct{ 1, 1 };
	FooMock fooMock;

	fooMock.InjectResetMyStruct(ResetMyStructSubstitute); // Mocking methods injection.
	fooMock.ResetMyStruct(myStruct);
	EXPECT_EQ(myStruct.x, 10);
	EXPECT_EQ(myStruct.y, 10);
}

TEST(Mockingbird, ConstReturnPrimitivesSignature){
	MyStruct myStruct{ 1, 1 };
	FooMock fooMock;

	fooMock.InjectCreateMyStruct(CreateMyStructSubstitute); // Mocking methods injection.
	auto createdMyStruct = fooMock.CreateMyStruct(10, 10);
	EXPECT_EQ(createdMyStruct.x, 20);
	EXPECT_EQ(createdMyStruct.y, 20);
}

TEST(Mockingbird, Overloading) {
	MyStruct myStruct{ 1, 1 };
	FooMock fooMock;

	fooMock.InjectCreateMyStruct(CreateMyStructSubstitute); // Mocking methods injection.
	auto createdMyStruct1 = fooMock.CreateMyStruct(10, 10);
	EXPECT_EQ(createdMyStruct1.x, 20);
	EXPECT_EQ(createdMyStruct1.y, 20);

	fooMock.InjectCreateMyStruct(CreateMyStructSubstitute2); // Mocking methods injection.
	auto createdMyStruct2 = fooMock.CreateMyStruct(0);
	EXPECT_EQ(createdMyStruct2.x, 5);
	EXPECT_EQ(createdMyStruct2.y, 5);
}

TEST(Mockingbird, ConstMethodConstRefPointerSignature){
	MyStruct myStruct{ 1, 1 };
	FooMock fooMock;

	fooMock.InjectMakeSpecialCopyMyStruct(MakeSpecialCopyMyStructSubstitute); // Mocking methods injection.
	auto specialCopy = fooMock.MakeSpecialCopyMyStruct(std::make_shared<MyStruct>(myStruct));
	EXPECT_EQ(specialCopy.x, 11);
	EXPECT_EQ(specialCopy.y, 11);
}

TEST(Mockingbird, OverloadConstMethod) {
	MyStruct myStruct{ 1, 1 };
	FooMock fooMock;

	fooMock.InjectMakeSpecialCopyMyStruct(MakeSpecialCopyMyStructSubstitute); // Mocking methods injection.
	auto specialCopy = fooMock.MakeSpecialCopyMyStruct(std::make_shared<MyStruct>(myStruct));
	EXPECT_EQ(specialCopy.x, 11);
	EXPECT_EQ(specialCopy.y, 11);

	fooMock.InjectMakeSpecialCopyMyStruct(MakeSpecialCopyMyStructSubstitute2); // Mocking methods injection.
	auto specialCopy2 = fooMock.MakeSpecialCopyMyStruct(myStruct);
	EXPECT_EQ(specialCopy2.x, 16);
	EXPECT_EQ(specialCopy2.y, 16);
}

TEST(Mockingbird, PassingMockPolymorphism){
	MyStruct myStruct{ 1, 1 };
	FooMock fooMock;

	// Mocking methods injection.
	fooMock.InjectResetMyStruct(ResetMyStructSubstitute);
	fooMock.InjectCreateMyStruct(CreateMyStructSubstitute);
	fooMock.InjectMakeSpecialCopyMyStruct(&MakeSpecialCopyMyStructSubstitute);

	auto createdMyStruct2 = GetMyStructFoo(std::make_unique<FooMock>(fooMock), 5, 5);
	EXPECT_EQ(createdMyStruct2.x, 20);
	EXPECT_EQ(createdMyStruct2.y, 20);
}

TEST(Mockingbird, NoMock){
	FooMock fooMock;
	EXPECT_EQ(fooMock.GetTen(), 10); // not injected so it returns the value of the base class.
}

TEST(Mockingbird, CallsCount) {
	MyStruct myStruct{ 1, 1 };
	FooMock fooMock;

	fooMock.InjectResetMyStruct(ResetMyStructSubstitute); // Mocking methods injection.
	EXPECT_EQ(fooMock.GetResetMyStructCallCounter(), 0);
	fooMock.ResetMyStruct(myStruct);
	EXPECT_EQ(fooMock.GetResetMyStructCallCounter(), 1);
	fooMock.ResetMyStruct(myStruct);
	EXPECT_EQ(fooMock.GetResetMyStructCallCounter(), 2);
}