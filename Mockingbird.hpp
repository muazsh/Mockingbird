/*
 *  Mockingbird is a simple and a powerful mocking framework for c++.
 *  Copyright (c) Mouaz Chamieh April 2021.
 *  Distributed under the MIT License. Please refer to the LICENSE file at:
 *  https://github.com/muazsh/Mockingbird
 */

#pragma once
#ifndef MOCKINGBIRD
#define MOCKINGBIRD

#include <functional>
#include <list>

#define ADD_TYPENAME1(x) typename x
#define ADD_TYPENAME2(x1, x2) typename x1, ADD_TYPENAME1(x2) 
#define ADD_TYPENAME3(x1, x2, x3) typename x1, ADD_TYPENAME2(x2, x3) 
#define ADD_TYPENAME4(x1, x2, x3, x4) typename x1, ADD_TYPENAME3(x2, x3, x4) 
#define ADD_TYPENAME5(x1, x2, x3, x4, x5) typename x1, ADD_TYPENAME1(x2, x3, x4, x5)

#define SELECT_MACRO(_1, _2, _3, _4, _5, NAME, ...) NAME
#define ADD_TYPENAME(...) SELECT_MACRO(__VA_ARGS__, ADD_TYPENAME5, ADD_TYPENAME4, ADD_TYPENAME3, ADD_TYPENAME2, ADD_TYPENAME1)(__VA_ARGS__)

#define CAT(param1,param2,param3,param4,param5,param6,param7,param8,param9,param10,param11,param12,param13,param14,param15,param16,param17,param18,param19,param20,...) param1##param2##param3##param4##param5##param6##param7##param8##param9##param10##param11##param12##param13##param14##param15##param16##param17##param18##param19##param20
#define CONCAT(...) CAT(__VA_ARGS__,,,,,,,,,,,,,,,,,,,,,,) //https://stackoverflow.com/a/74729170/1149948

#define START_MOCK_TEMPLATE(MockingClass, MockedClass, ...)                                                                           \
template<ADD_TYPENAME(__VA_ARGS__)>                                                                                                   \
class MockingClass : public MockedClass<__VA_ARGS__> {                          

#define FUNCTION_BODY(Counter, Member, ...)                                                                                           \
Counter++;                                                                                                                            \
if(Member.m_call_once.empty())                                                                                                        \
	return Member.m_func(__VA_ARGS__);                                                                                                \
auto lastElem = Member.m_call_once.back();                                                                                            \
Member.m_call_once.pop_back();                                                                                                        \
return lastElem(__VA_ARGS__);

#define FUNCTION_INJECTION_SET(FuncName, Substitute)                                                                                  \
private:                                                                                                                              \
template<class Type>                                                                                                                  \
class CONCAT(FuncName, Class) {                                                                                                       \
	public:                                                                                                                           \
		CONCAT(FuncName, Class)():m_func((Type)Substitute){}                                                                          \
		Type m_func;                                                                                                                  \
        mutable std::list<Type> m_call_once;                                                                                          \
};                                                                                                                                    \
CONCAT(FuncName, Class)<decltype(Substitute)> CONCAT(m_, FuncName, Class);                                                            \
mutable int CONCAT(m_, FuncName, CallCounter) = 0;                                                                                    \
public:                                                                                                                               \
void CONCAT(Inject, FuncName)(decltype(Substitute) sub){                                                                              \
CONCAT(m_, FuncName, Class).m_func = sub;                                                                                             \
}                                                                                                                                     \
void CONCAT(Inject, FuncName, CallOnce)(decltype(Substitute) sub){                                                                    \
CONCAT(m_, FuncName, Class).m_call_once.push_front(sub);                                                                              \
}                                                                                                                                     \
int CONCAT(Get, FuncName, CallCounter)(){return CONCAT(m_, FuncName, CallCounter);                                                    \
}

#define FUNCTION_OVERLOAD_INJECTION_SET(FuncName, Substitute, Disc)                                                                   \
private:                                                                                                                              \
FuncName##Class<decltype(Substitute)> m_##FuncName##Class##Disc;                                                                      \
mutable int m_##FuncName##Disc##CallCounter = 0;                                                                                      \
                                                                                                                                      \
public:                                                                                                                               \
void Inject##FuncName(decltype(Substitute) sub){                                                                                      \
m_##FuncName##Class##Disc.m_func = sub;                                                                                               \
}                                                                                                                                     \
void Inject##FuncName##CallOnce(decltype(Substitute) sub){                                                                            \
m_##FuncName##Class##Disc.m_call_once.push_front(sub);                                                                                \
}                                                                                                                                     \
int Get##FuncName##Disc##CallCounter(){return m_##FuncName##Disc##CallCounter;}

#define START_MOCK(MockingClass, MockedClass)                                                                                         \
class MockingClass : public MockedClass {                                                                                             
                                                                                                                                     
#define FUNCTION(FuncName,ReturnType, Signature, ExpressionWithoutCommas, .../*signature parameters*/)                                \
private:                                                                                                                              \
static ReturnType FuncName##Type Signature {ExpressionWithoutCommas;}                                                                 \
FUNCTION_INJECTION_SET(FuncName, & FuncName##Type)                                                                                    \
ReturnType FuncName Signature {                                                                                                       \
FUNCTION_BODY(m_##FuncName##CallCounter, m_##FuncName##Class, __VA_ARGS__)                                                            \
}

#define FUNCTION_CONST(FuncName, ReturnType, Signature, ExpressionWithoutCommas, .../*signature parameters*/)                         \
private:                                                                                                                              \
static ReturnType FuncName##Type Signature {ExpressionWithoutCommas;}                                                                 \
FUNCTION_INJECTION_SET(FuncName, & FuncName##Type)                                                                                    \
public:                                                                                                                               \
ReturnType FuncName Signature const {                                                                                                 \
FUNCTION_BODY(m_##FuncName##CallCounter, m_##FuncName##Class, __VA_ARGS__)                                                            \
}

#define FUNCTION_OVERLOAD(FuncName,ReturnType, Signature, ExpressionWithoutCommas, Disc, .../*signature parameters*/)                 \
private:                                                                                                                              \
static ReturnType FuncName##Type##Disc Signature {ExpressionWithoutCommas;}                                                           \
FUNCTION_OVERLOAD_INJECTION_SET(FuncName, & FuncName##Type##Disc, Disc)                                                               \
ReturnType FuncName Signature {                                                                                                       \
FUNCTION_BODY(m_##FuncName##Disc##CallCounter, m_##FuncName##Class##Disc, __VA_ARGS__)                                                \
}

#define FUNCTION_OVERLOAD_CONST(FuncName, ReturnType, Signature, ExpressionWithoutCommas, Disc, ...)                                  \
private:                                                                                                                              \
static ReturnType FuncName##Type##Disc Signature {ExpressionWithoutCommas;}                                                           \
FUNCTION_OVERLOAD_INJECTION_SET(FuncName, & FuncName##Type##Disc, Disc)                                                               \
ReturnType FuncName Signature const {                                                                                                 \
FUNCTION_BODY(m_##FuncName##Disc##CallCounter, m_##FuncName##Class##Disc, __VA_ARGS__)                                                \
}

#define FUNC(FuncName,ReturnType, Signature, Substitute, .../*signature parameters*/)                                                 \
FUNCTION_INJECTION_SET(CONCAT(FuncName, _, __VA_ARGS__) , Substitute)                                                                 \
ReturnType FuncName Signature {                                                                                                       \
FUNCTION_BODY(CONCAT(m_, FuncName, _, __VA_ARGS__, CallCounter), CONCAT(m_, FuncName, _, __VA_ARGS__, Class), __VA_ARGS__)            \
}

#define END_MOCK(MockingClass)                                                                                                        \
private:                                                                                                                              \
std::function<void(void)> m_dtor = [](){};                                                                                            \
public:                                                                                                                               \
void InjectDestructor(std::function<void(void)> dtor) { m_dtor = dtor; }                                                              \
~MockingClass(){m_dtor();}                                                                                                            \
};

#endif // !MOCKINGBIRD