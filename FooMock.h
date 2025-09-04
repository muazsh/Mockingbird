#pragma once

#include "Foo.h"
#include "Mockingbird.hpp"


START_MOCK(FooMock, Foo)
FUNCTION(ResetMyStruct, void, (MyStruct& myStruct),, override, return, myStruct)
FUNCTION(CreateMyStruct, const MyStruct, (int x, int y),, override, return MyStruct{}, x, y)
FUNCTION_OVERLOAD(CreateMyStruct, const MyStruct, (int x),, override, return MyStruct{}, 1, x)
FUNCTION(MakeSpecialCopyMyStruct, MyStruct, (const std::shared_ptr<MyStruct>&myStructPtr), const, override, return MyStruct{}, myStructPtr)
FUNCTION_OVERLOAD(MakeSpecialCopyMyStruct, MyStruct, (const MyStruct & myStruct), const, override, return MyStruct{}, 1, myStruct)
FUNCTION_OVERLOAD(MakeSpecialCopyMyStruct, MyStruct, (MyStruct && myStructRLR), const, override, return MyStruct{}, 2, std::move(myStructRLR))
FUNCTION(GetString, std::string, (),,, return "")
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
FUNCTION(MakeSpecialCopyMyStruct, MyStruct, (MyStruct&& myStructRLR), const, override, return MyStruct{}, std::move(myStructRLR))
FUNC(GetString, std::string, (), &GetStringDummy)
END_MOCK(FooMock2)

START_MOCK_TEMPLATE(FooTemplatedMock, TemplatedFoo, T, E)
FUNCTION(Sum, T, (T x, E y),, override, return T{}, x, y)
FUNCTION_OVERLOAD(Sum, T, (T x, E y, T z),, override, return T{}, OverloadWithZ, x, y, z)
FUNCTION(SumConst, T, (T x, E y), const, override, return T{}, x, y)
FUNCTION_OVERLOAD(SumConst, T, (T x, E y, int&& z), const, override, return T{}, RvalRef, x, y, std::move(z))
END_MOCK(FooTemplatedMock)
