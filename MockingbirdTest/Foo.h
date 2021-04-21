#pragma once
#include<memory>

struct MyStruct {
	int x, y;
};

class Foo {
public:
	virtual void ResetMyStruct(MyStruct& myStruct) = 0;
	virtual const MyStruct CreateMyStruct(int x, int y) { return MyStruct{ x,y }; };
	virtual const MyStruct CreateMyStruct(int x) { return MyStruct{ x,x }; };
	virtual MyStruct MakeSpecialCopyMyStruct(const std::shared_ptr<MyStruct>& myStruct) const { return MyStruct{ myStruct->x, myStruct->y }; }
	virtual MyStruct MakeSpecialCopyMyStruct(const MyStruct& myStruct) const { return MyStruct{ myStruct.x, myStruct.y }; }
	virtual int GetTen() { return 10; }
	std::string GetString() { return "Original"; }
};

MyStruct GetMyStructFoo(const std::unique_ptr<Foo>& foo, int x, int y) {
	auto myStruct = foo->CreateMyStruct(x, y);
	foo->ResetMyStruct(myStruct);
	return foo->MakeSpecialCopyMyStruct(std::make_shared<MyStruct>(myStruct));
}

std::string GetStringFoo(Foo& foo) {
	return foo.GetString();
}
