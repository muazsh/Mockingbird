#pragma once

#include "Foo.h"
#include "Mockingbird.hpp"

void ResetMyStructDummy(MyStruct& myStruct) {}
const MyStruct CreateMyStructDummy(int x, int y) { return MyStruct{ 0,0 }; }
const MyStruct CreateMyStructDummy2(int x) { return MyStruct{ 0,0 }; }
MyStruct MakeSpecialCopyMyStructDummy(const std::shared_ptr<MyStruct>& myStruct) { return MyStruct{ 0,0 }; } // This is a static method wihch cannot be const
MyStruct MakeSpecialCopyMyStructDummy2(const MyStruct& myStruct) { return MyStruct{ 0,0 }; }

START_MOCK(FooMock, Foo)
FUNCTION(ResetMyStruct, void, (MyStruct& myStruct), &ResetMyStructDummy, myStruct)
FUNCTION(CreateMyStruct, const MyStruct, (int x, int y), &CreateMyStructDummy, x, y)
FUNCTION_OVERLOADING(CreateMyStruct, const MyStruct, (int x), &CreateMyStructDummy2, 1, x)
CONST_FUNCTION(MakeSpecialCopyMyStruct, MyStruct, (const std::shared_ptr<MyStruct>& myStruct), &MakeSpecialCopyMyStructDummy, myStruct)
CONST_FUNCTION_OVERLOADING(MakeSpecialCopyMyStruct, MyStruct, (const MyStruct& myStruct), &MakeSpecialCopyMyStructDummy2, 1, myStruct)
END_MOCK(FooMock)
