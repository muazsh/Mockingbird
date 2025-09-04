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

START_MOCK_TEMPLATE(FooTemplatedMock, TemplatedFoo, T, E)
FUNCTION(Sum, T, (T x, E y),, override, return T{}, x, y)
FUNCTION_OVERLOAD(Sum, T, (T x, E y, T z),, override, return T{}, OverloadWithZ, x, y, z)
FUNCTION(SumConst, T, (T x, E y), const, override, return T{}, x, y)
FUNCTION_OVERLOAD(SumConst, T, (T x, E y, int&& z), const, override, return T{}, RvalRef, x, y, std::move(z))
END_MOCK(FooTemplatedMock)
