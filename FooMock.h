#pragma once

#include "Foo.h"
#include "Mockingbird.hpp"

void ResetMyStructDummy(MyStruct& myStruct) {}
const MyStruct CreateMyStructDummy(int x, int y) { return MyStruct{ 0,0 }; }
const MyStruct CreateMyStructDummy1(int x) { return MyStruct{ 0,0 }; }
MyStruct MakeSpecialCopyMyStructDummy(const std::shared_ptr<MyStruct>& myStruct) { return MyStruct{ 0,0 }; } // This is a static method wihch cannot be const
MyStruct MakeSpecialCopyMyStructDummy1(const MyStruct& myStruct) { return MyStruct{ 0,0 }; }
MyStruct MakeSpecialCopyMyStructDummy2(MyStruct&& myStruct) { return MyStruct{ 0,0 }; }
std::string GetStringDummy() { return ""; }

START_MOCK(FooMock, Foo)
FUNCTION(ResetMyStruct, void, (MyStruct& myStruct), &ResetMyStructDummy, myStruct)
FUNCTION(CreateMyStruct, const MyStruct, (int x, int y), &CreateMyStructDummy, x, y)
FUNCTION_OVERLOAD(CreateMyStruct, const MyStruct, (int x), &CreateMyStructDummy1, 1, x)
FUNCTION(MakeSpecialCopyMyStruct, MyStruct, (const std::shared_ptr<MyStruct>& myStructPtr)const, &MakeSpecialCopyMyStructDummy, myStructPtr)
FUNCTION_OVERLOAD(MakeSpecialCopyMyStruct, MyStruct, (const MyStruct& myStruct)const, &MakeSpecialCopyMyStructDummy1, 1, myStruct)
FUNCTION_OVERLOAD(MakeSpecialCopyMyStruct, MyStruct, (MyStruct&& myStructRLR)const, &MakeSpecialCopyMyStructDummy2, 2, std::move(myStructRLR))
FUNCTION(GetString, std::string, (), &GetStringDummy)
END_MOCK(FooMock)

START_MOCK(FooMock2, Foo)
FUNC(ResetMyStruct, void, (MyStruct& myStruct), &ResetMyStructDummy, myStruct)
FUNC(CreateMyStruct, const MyStruct, (int x, int y), &CreateMyStructDummy, x, y)
FUNC(CreateMyStruct, const MyStruct, (int x), &CreateMyStructDummy1, x)
FUNC(MakeSpecialCopyMyStruct, MyStruct, (const std::shared_ptr<MyStruct>& myStructPtr)const, &MakeSpecialCopyMyStructDummy, myStructPtr)
FUNC(MakeSpecialCopyMyStruct, MyStruct, (const MyStruct& myStruct)const, &MakeSpecialCopyMyStructDummy1, myStruct)
FUNCTION(MakeSpecialCopyMyStruct, MyStruct, (MyStruct&& myStructRLR)const, &MakeSpecialCopyMyStructDummy2, std::move(myStructRLR))
FUNC(GetString, std::string, (), &GetStringDummy)
END_MOCK(FooMock2)

START_MOCK_TEMPLATE(FooTemplatedMock, TemplatedFoo, T, E)
FUNCTION_TEMPLATE(Sum, T, (T x, E y), return 0, x, y)
FUNCTION_TEMPLATE_OVERLOADING(Sum, T, (T x, E y, T z), return 0, 1, x, y, z)
FUNCTION_TEMPLATE_CONST(SumConst, T, (T x, E y), return 0, x, y)
FUNCTION_TEMPLATE_OVERLOADING_CONST(SumConst, T, (T x, E y, int&& z), return 0, 1, x, y, std::move(z))
END_MOCK(FooTemplatedMock)
