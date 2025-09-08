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

#define SELECT_MACRO(_1, _2, _3, _4, _5, NAME, ...) NAME
#define PREPEND_EXPR1(expr, x) expr x
#define PREPEND_EXPR2(expr, x1, x2) expr x1, PREPEND_EXPR1(expr, x2) 
#define PREPEND_EXPR3(expr, x1, x2, x3) expr x1, PREPEND_EXPR2(expr, x2, x3) 
#define PREPEND_EXPR4(expr, x1, x2, x3, x4) expr x1, PREPEND_EXPR3(expr, x2, x3, x4) 
#define PREPEND_EXPR5(expr, x1, x2, x3, x4, x5) expr x1, PREPEND_EXPR4(expr, x2, x3, x4, x5)
#define PREPEND_EXPR(expr, ...) SELECT_MACRO(__VA_ARGS__, PREPEND_EXPR5, PREPEND_EXPR4, PREPEND_EXPR3, PREPEND_EXPR2, PREPEND_EXPR1)(expr, __VA_ARGS__)

#define START_MOCK_TEMPLATE(MockingClass, MockedClass, ...)                                                                           \
template<PREPEND_EXPR(typename, __VA_ARGS__)>                                                                                         \
class MockingClass : public MockedClass<__VA_ARGS__> {                          

#define FUNCTION_BODY(Counter, Member, ...)                                                                                           \
Counter++;                                                                                                                            \
if(Member.m_CallOnce.empty())                                                                                                         \
	return Member.m_Func(__VA_ARGS__);                                                                                                \
auto lastElem = Member.m_CallOnce.back();                                                                                             \
Member.m_CallOnce.pop_back();                                                                                                         \
return lastElem(__VA_ARGS__);

#define FUNCTION_INJECTION_SET(FuncName, Substitute)                                                                                  \
private:                                                                                                                              \
template<class Type>                                                                                                                  \
class FuncName##Class {                                                                                                               \
	public:                                                                                                                           \
		FuncName##Class():m_Func((Type)Substitute){}                                                                                  \
		Type m_Func;                                                                                                                  \
        mutable std::list<Type> m_CallOnce;                                                                                           \
};                                                                                                                                    \
FuncName##Class<decltype(Substitute)> m_##FuncName##Class;                                                                            \
mutable unsigned m_##FuncName##CallCounter = 0;                                                                                       \
public:                                                                                                                               \
void Inject##FuncName(decltype(Substitute) sub){                                                                                      \
m_##FuncName##Class.m_Func = sub;                                                                                                     \
}                                                                                                                                     \
void Inject##FuncName##CallOnce(decltype(Substitute) sub){                                                                            \
m_##FuncName##Class.m_CallOnce.push_front(sub);                                                                                       \
}                                                                                                                                     \
int Get##FuncName##CallCounter(){return m_##FuncName##CallCounter;                                                                    \
}

#define FUNCTION_OVERLOAD_INJECTION_SET(FuncName, Substitute, Disc)                                                                   \
private:                                                                                                                              \
FuncName##Class<decltype(Substitute)> m_##FuncName##Class##Disc;                                                                      \
mutable int m_##FuncName##Disc##CallCounter = 0;                                                                                      \
                                                                                                                                      \
public:                                                                                                                               \
void Inject##FuncName(decltype(Substitute) sub){                                                                                      \
m_##FuncName##Class##Disc.m_Func = sub;                                                                                               \
}                                                                                                                                     \
void Inject##FuncName##CallOnce(decltype(Substitute) sub){                                                                            \
m_##FuncName##Class##Disc.m_CallOnce.push_front(sub);                                                                                 \
}                                                                                                                                     \
int Get##FuncName##Disc##CallCounter(){return m_##FuncName##Disc##CallCounter;}

#define START_MOCK(MockingClass, ...)                                                                                                 \
class MockingClass : PREPEND_EXPR(public, __VA_ARGS__) {                                                                                             

#define FUNCTION(FuncName,ReturnType, Signature, Const, Override, ExpressionWithoutCommas, .../*signature parameters*/)               \
private:                                                                                                                              \
static ReturnType FuncName##Type Signature {ExpressionWithoutCommas;}                                                                 \
FUNCTION_INJECTION_SET(FuncName, & FuncName##Type)                                                                                    \
ReturnType FuncName Signature Const Override {                                                                                        \
FUNCTION_BODY(m_##FuncName##CallCounter, m_##FuncName##Class, __VA_ARGS__)                                                            \
}

#define FUNCTION_OVERLOAD(FuncName,ReturnType, Signature, Const, Override, ExpressionWithoutCommas, Disc, .../*signature parameters*/)\
private:                                                                                                                              \
static ReturnType FuncName##Type##Disc Signature {ExpressionWithoutCommas;}                                                           \
FUNCTION_OVERLOAD_INJECTION_SET(FuncName, & FuncName##Type##Disc, Disc)                                                               \
ReturnType FuncName Signature Const Override{                                                                                         \
FUNCTION_BODY(m_##FuncName##Disc##CallCounter, m_##FuncName##Class##Disc, __VA_ARGS__)                                                \
}

#define END_MOCK(MockingClass)                                                                                                        \
private:                                                                                                                              \
std::function<void(void)> m_dtor = [](){};                                                                                            \
public:                                                                                                                               \
void InjectDestructor(std::function<void(void)> dtor) { m_dtor = dtor; }                                                              \
~MockingClass(){m_dtor();}                                                                                                            \
};

#endif // !MOCKINGBIRD