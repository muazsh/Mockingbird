#pragma once
#ifndef MOCKINGBIRD
#define MOCKINGBIRD

#define START_MOCK(MockingClass, MockedClass)\
class MockingClass : public MockedClass {\
 public: 

#define INJECTION_SET(FuncName, Substitute)\
template<class T>\
class FuncName##Class {\
public:\
T m_func;\
};\
private:\
FuncName##Class<decltype(Substitute)> m_##FuncName##Class;\
public:\
void Inject##FuncName(decltype(Substitute)){\
m_##FuncName##Class.m_func = Substitute;}

#define METHOD_INJECTION_SET(FuncName,ReturnType, Signature, Substitute, .../*signature variables*/)\
INJECTION_SET(FuncName, Substitute)\
ReturnType FuncName##Signature override{return m_##FuncName##Class.m_func(__VA_ARGS__);}

#define CONST_METHOD_INJECTION_SET(FuncName,ReturnType, Signature, Substitute, .../*signature variables*/)\
INJECTION_SET(FuncName, Substitute)\
ReturnType FuncName##Signature const override{return m_##FuncName##Class.m_func(__VA_ARGS__);}

#define END_MOCK(MockingClass)\
};

#endif // !MOCKINGBIRD
