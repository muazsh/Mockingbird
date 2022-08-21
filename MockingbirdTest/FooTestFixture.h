#pragma once

#include "Foo.h"
#include "Mockingbird.h"

void ResetMyStructDummy(MyStruct& myStruct) {}
const MyStruct CreateMyStructDummy(int x, int y) { return MyStruct{ 0,0 }; }
const MyStruct CreateMyStructDummy1(int x) { return MyStruct{ 0,0 }; }
MyStruct MakeSpecialCopyMyStructDummy(const std::shared_ptr<MyStruct>& myStruct) { return MyStruct{ 0,0 }; } // This is a static method wihch cannot be const
MyStruct MakeSpecialCopyMyStructDummy1(const MyStruct& myStruct) { return MyStruct{ 0,0 }; }
std::string GetStringDummy() { return ""; }

START_MOCK(FooMock, Foo)
FUNCTION(ResetMyStruct, void, (MyStruct& myStruct), &ResetMyStructDummy, myStruct)
FUNCTION(CreateMyStruct, const MyStruct, (int x, int y), &CreateMyStructDummy, x, y)
FUNCTION_OVERLOADING(CreateMyStruct, const MyStruct, (int x), &CreateMyStructDummy1, 1, x)
FUNCTION_CONST(MakeSpecialCopyMyStruct, MyStruct, (const std::shared_ptr<MyStruct>& myStruct), &MakeSpecialCopyMyStructDummy, myStruct)
FUNCTION_CONST_OVERLOADING(MakeSpecialCopyMyStruct, MyStruct, (const MyStruct& myStruct), &MakeSpecialCopyMyStructDummy1, 1, myStruct)
HIDE(GetString, std::string, (), &GetStringDummy)
END_MOCK(FooMock)
