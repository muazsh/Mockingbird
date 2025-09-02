#pragma once

#include "Foo.h"
#include "Mockingbird.hpp"


START_MOCK(FooMock, Foo)
FUNCTION(ResetMyStruct, void, (MyStruct& myStruct), return, myStruct)
FUNCTION(CreateMyStruct, const MyStruct, (int x, int y), return MyStruct{}, x, y)
FUNCTION_OVERLOAD(CreateMyStruct, const MyStruct, (int x), return MyStruct{}, 1, x)
FUNCTION_CONST(MakeSpecialCopyMyStruct, MyStruct, (const std::shared_ptr<MyStruct>&myStructPtr), return MyStruct{}, myStructPtr)
FUNCTION_OVERLOAD_CONST(MakeSpecialCopyMyStruct, MyStruct, (const MyStruct & myStruct), return MyStruct{}, 1, myStruct)
FUNCTION_OVERLOAD_CONST(MakeSpecialCopyMyStruct, MyStruct, (MyStruct && myStructRLR), return MyStruct{}, 2, std::move(myStructRLR))
FUNCTION(GetString, std::string, (), return "")
END_MOCK(FooMock)

void ResetMyStructDummy(MyStruct& myStruct) {}
const MyStruct CreateMyStructDummy(int x, int y) { return MyStruct{ 0,0 }; }
const MyStruct CreateMyStructDummy1(int x) { return MyStruct{ 0,0 }; }
MyStruct MakeSpecialCopyMyStructDummy(const std::shared_ptr<MyStruct>& myStruct) { return MyStruct{ 0,0 }; } 
MyStruct MakeSpecialCopyMyStructDummy1(const MyStruct& myStruct) { return MyStruct{ 0,0 }; }
std::string GetStringDummy() { return ""; }

START_MOCK(FooMock2, Foo)
FUNC(ResetMyStruct, void, (MyStruct& myStruct), &ResetMyStructDummy, myStruct)
FUNC(CreateMyStruct, const MyStruct, (int x, int y), &CreateMyStructDummy, x, y)
FUNC(CreateMyStruct, const MyStruct, (int x), &CreateMyStructDummy1, x)
FUNC(MakeSpecialCopyMyStruct, MyStruct, (const std::shared_ptr<MyStruct>& myStructPtr)const, &MakeSpecialCopyMyStructDummy, myStructPtr)
FUNC(MakeSpecialCopyMyStruct, MyStruct, (const MyStruct& myStruct)const, &MakeSpecialCopyMyStructDummy1, myStruct)
FUNCTION_CONST(MakeSpecialCopyMyStruct, MyStruct, (MyStruct&& myStructRLR), return MyStruct{}, std::move(myStructRLR))
FUNC(GetString, std::string, (), &GetStringDummy)
END_MOCK(FooMock2)

START_MOCK_TEMPLATE(FooTemplatedMock, TemplatedFoo, T, E)
FUNCTION(Sum, T, (T x, E y), return T{}, x, y)
FUNCTION_OVERLOAD(Sum, T, (T x, E y, T z), return T{}, OverloadWithZ, x, y, z)
FUNCTION_CONST(SumConst, T, (T x, E y), return T{}, x, y)
FUNCTION_OVERLOAD_CONST(SumConst, T, (T x, E y, int&& z), return T{}, RvalRef, x, y, std::move(z))
END_MOCK(FooTemplatedMock)
