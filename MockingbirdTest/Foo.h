#pragma once
#include "MyStruct.h"
#include <memory>
#include <string>

class Foo {
public:
	virtual void ResetMyStruct(MyStruct& myStruct) = 0;
	virtual const MyStruct CreateMyStruct(int x, int y) { return MyStruct{ x,y }; };
	virtual const MyStruct CreateMyStruct(int x) { return MyStruct{ x,x }; };
	virtual MyStruct MakeSpecialCopyMyStruct(const std::shared_ptr<MyStruct>& myStruct) const { return MyStruct{ myStruct->x, myStruct->y }; }
	virtual MyStruct MakeSpecialCopyMyStruct(const MyStruct& myStruct) const { return MyStruct{ myStruct.x, myStruct.y }; }
	virtual MyStruct MakeSpecialCopyMyStruct(MyStruct&& myStruct) const { return MyStruct{ myStruct.x, myStruct.y }; }
	virtual int GetTen() { return 10; }
	std::string GetString() { return "Original"; }
	~Foo() {}
};

template<class T, class E>
class TemplatedFoo {
public:
	virtual T Sum(T x, E y) = 0;
	virtual T SumConst(T x, E y) const = 0;
	virtual T Sum(T x, E y, T z) = 0;
	~TemplatedFoo() {}
};