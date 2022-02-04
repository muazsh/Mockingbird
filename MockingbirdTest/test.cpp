#include "pch.h"
#include "FooTestFixture.h"

// Substitutes
void ResetMyStructSubstitute(MyStruct& myStruct) { myStruct.x = 10; myStruct.y = 10; }
const MyStruct CreateMyStructSubstitute(int x, int y) { return MyStruct{ x + 10, y + 10 }; }
const MyStruct CreateMyStructSubstitute2(int x) { return MyStruct{ x + 5, x + 5 }; }
MyStruct MakeSpecialCopyMyStructSubstitute(const std::shared_ptr<MyStruct>& myStruct) { return MyStruct{ myStruct->x + 10, myStruct->y + 10 }; } // This is a static method wihch cannot be const
MyStruct MakeSpecialCopyMyStructSubstitute2(const MyStruct& myStruct) { return MyStruct{ myStruct.x + 15, myStruct.y + 15 }; }
std::string GetStringSubstitute() { return "Mock"; }

// Functions to test Polymorphism.
MyStruct GetMyStructFoo(const std::unique_ptr<Foo>& foo, int x, int y) {
	auto myStruct = foo->CreateMyStruct(x, y);
	foo->ResetMyStruct(myStruct);
	return foo->MakeSpecialCopyMyStruct(std::make_shared<MyStruct>(myStruct));
}

std::string GetStringFoo(Foo& foo) {
	return foo.GetString();
}

TEST(Mockingbird, VoidReturnReferenceSignature){
	MyStruct myStruct{ 1, 1 };
	FooMock fooMock;

	fooMock.InjectResetMyStruct(ResetMyStructSubstitute); // Mocking methods injection.
	fooMock.ResetMyStruct(myStruct);
	EXPECT_EQ(10, myStruct.x);
	EXPECT_EQ(10, myStruct.y);
}

TEST(Mockingbird, ConstReturnPrimitivesSignature){
	MyStruct myStruct{ 1, 1 };
	FooMock fooMock;

	fooMock.InjectCreateMyStruct(CreateMyStructSubstitute); // Mocking methods injection.
	auto createdMyStruct = fooMock.CreateMyStruct(10, 10);
	EXPECT_EQ(20, createdMyStruct.x);
	EXPECT_EQ(20, createdMyStruct.y);
}

TEST(Mockingbird, Overloading) {
	MyStruct myStruct{ 1, 1 };
	FooMock fooMock;

	fooMock.InjectCreateMyStruct(CreateMyStructSubstitute); // Mocking methods injection.
	auto createdMyStruct1 = fooMock.CreateMyStruct(10, 10);
	EXPECT_EQ(20, createdMyStruct1.x);
	EXPECT_EQ(20, createdMyStruct1.y);

	fooMock.InjectCreateMyStruct(CreateMyStructSubstitute2); // Mocking methods injection.
	auto createdMyStruct2 = fooMock.CreateMyStruct(0);
	EXPECT_EQ(5, createdMyStruct2.x);
	EXPECT_EQ(5, createdMyStruct2.y);
}

TEST(Mockingbird, ConstMethodConstRefPointerSignature){
	MyStruct myStruct{ 1, 1 };
	FooMock fooMock;

	fooMock.InjectMakeSpecialCopyMyStruct(MakeSpecialCopyMyStructSubstitute); // Mocking methods injection.
	auto specialCopy = fooMock.MakeSpecialCopyMyStruct(std::make_shared<MyStruct>(myStruct));
	EXPECT_EQ(11, specialCopy.x);
	EXPECT_EQ(11, specialCopy.y);
}

TEST(Mockingbird, OverloadConstMethod) {
	MyStruct myStruct{ 1, 1 };
	FooMock fooMock;

	fooMock.InjectMakeSpecialCopyMyStruct(MakeSpecialCopyMyStructSubstitute); // Mocking methods injection.
	auto specialCopy = fooMock.MakeSpecialCopyMyStruct(std::make_shared<MyStruct>(myStruct));
	EXPECT_EQ(11, specialCopy.x);
	EXPECT_EQ(11, specialCopy.y);

	fooMock.InjectMakeSpecialCopyMyStruct(MakeSpecialCopyMyStructSubstitute2); // Mocking methods injection.
	auto specialCopy2 = fooMock.MakeSpecialCopyMyStruct(myStruct);
	EXPECT_EQ(16, specialCopy2.x);
	EXPECT_EQ(16, specialCopy2.y);
}

TEST(Mockingbird, PassingMockPolymorphism){
	MyStruct myStruct{ 1, 1 };
	FooMock fooMock;

	// Mocking methods injection.
	fooMock.InjectResetMyStruct(ResetMyStructSubstitute);
	fooMock.InjectCreateMyStruct(CreateMyStructSubstitute);
	fooMock.InjectMakeSpecialCopyMyStruct(&MakeSpecialCopyMyStructSubstitute);

	auto createdMyStruct2 = GetMyStructFoo(std::make_unique<FooMock>(fooMock), 5, 5);
	EXPECT_EQ(20, createdMyStruct2.x);
	EXPECT_EQ(20, createdMyStruct2.y);
}

TEST(Mockingbird, Spy){
	FooMock fooMock;
	EXPECT_EQ(10, fooMock.GetTen()); // not injected so it returns the value of the base class.
}

TEST(Mockingbird, CallsCount) {
	MyStruct myStruct{ 1, 1 };
	FooMock fooMock;

	fooMock.InjectResetMyStruct(ResetMyStructSubstitute); // Mocking methods injection.
	EXPECT_EQ(0, fooMock.GetResetMyStructCallCounter());
	fooMock.ResetMyStruct(myStruct);
	EXPECT_EQ(1, fooMock.GetResetMyStructCallCounter());
	fooMock.ResetMyStruct(myStruct);
	EXPECT_EQ(2, fooMock.GetResetMyStructCallCounter());
}

TEST(Mockingbird, DefaultDummyFunctions) {
	MyStruct myStruct{ 4,4 };
	FooMock fooMock;

	auto createdMyStruct = fooMock.CreateMyStruct(10, 10);
	EXPECT_EQ(0, createdMyStruct.x);
	EXPECT_EQ(0, createdMyStruct.y);

	createdMyStruct = fooMock.CreateMyStruct(9);
	EXPECT_EQ(0, createdMyStruct.x);
	EXPECT_EQ(0, createdMyStruct.y);

	createdMyStruct = fooMock.MakeSpecialCopyMyStruct(myStruct);
	EXPECT_EQ(0, createdMyStruct.x);
	EXPECT_EQ(0, createdMyStruct.y);

	createdMyStruct = fooMock.MakeSpecialCopyMyStruct(std::make_shared<MyStruct>(myStruct));
	EXPECT_EQ(0, createdMyStruct.x);
	EXPECT_EQ(0, createdMyStruct.y);
	
	fooMock.ResetMyStruct(myStruct);
	EXPECT_EQ(0, createdMyStruct.x);
	EXPECT_EQ(0, createdMyStruct.y);
}

TEST(Mockingbird, Hide) {
	FooMock fooMock;

	fooMock.InjectGetString(GetStringSubstitute); // Mocking methods injection.
	EXPECT_EQ("Mock", fooMock.GetString());
	EXPECT_EQ("Original", GetStringFoo(fooMock)); // hidden function is not virtual, no polymorphism.  
	EXPECT_EQ(1, fooMock.GetGetStringCallCounter());
}