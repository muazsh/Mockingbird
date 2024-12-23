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

#define ADD_TYPENAME1(x) typename x
#define ADD_TYPENAME2(x1, x2) typename x1, ADD_TYPENAME1(x2) 
#define ADD_TYPENAME3(x1, x2, x3) typename x1, ADD_TYPENAME2(x2, x3) 
#define ADD_TYPENAME4(x1, x2, x3, x4) typename x1, ADD_TYPENAME3(x2, x3, x4) 
#define ADD_TYPENAME5(x1, x2, x3, x4, x5) typename x1, ADD_TYPENAME1(x2, x3, x4, x5)

#define SELECT_MACRO(_1, _2, _3, _4, _5, NAME, ...) NAME
#define ADD_TYPENAME(...) SELECT_MACRO(__VA_ARGS__, ADD_TYPENAME5, ADD_TYPENAME4, ADD_TYPENAME3, ADD_TYPENAME2, ADD_TYPENAME1)(__VA_ARGS__)

#define START_MOCK_TEMPLATE(MockingClass, MockedClass, ...)                                                                           \
template<ADD_TYPENAME(__VA_ARGS__)>                                                                                                                 \
class MockingClass : public MockedClass<__VA_ARGS__> {                          

#define FUNCTION_TEMPLATE_PREFIX(FuncName,...)                                                                                        \
private:                                                                                                                              \
mutable int m_##FuncName##CallCounter##__VA_ARGS__ = 0;                                                                               \
                                                                                                                                      \
public:                                                                                                                               \
int Get##FuncName##CallCounter##__VA_ARGS__(){return m_##FuncName##CallCounter##__VA_ARGS__;}

#define FUNCTION_TEMPLATE_SUFFIX(FuncName, Expression, ...)                                                                           \
{                                                                                                                                     \
m_##FuncName##CallCounter##__VA_ARGS__++;                                                                                             \
Expression;                                                                                                                           \
}

#define FUNCTION_TEMPLATE(FuncName,ReturnType, Signature, Expression, .../*signature parameters*/)                                    \
FUNCTION_TEMPLATE_PREFIX(FuncName)                                                                                                    \
ReturnType FuncName Signature override                                                                                                \
FUNCTION_TEMPLATE_SUFFIX(FuncName, Expression)                                                                                        \

#define FUNCTION_TEMPLATE_OVERLOADING(FuncName,ReturnType, Signature, Expression, overloadedMethodNumber, .../*signature variables*/) \
FUNCTION_TEMPLATE_PREFIX(FuncName, overloadedMethodNumber)                                                                            \
ReturnType FuncName Signature override                                                                                                \
FUNCTION_TEMPLATE_SUFFIX(FuncName, Expression, overloadedMethodNumber)                                  

#define START_MOCK(MockingClass, MockedClass)                                                                                         \
class MockingClass : public MockedClass {                                                                                             \
                                                                                                                                     
#define FUNCTION_INJECTION_SET(FuncName, Substitute)                                                                                  \
private:                                                                                                                              \
                                                                                                                                      \
template<class T>                                                                                                                     \
class FuncName##Class {                                                                                                               \
	public:                                                                                                                           \
		FuncName##Class():m_func((T)Substitute){}                                                                                     \
		T m_func;                                                                                                                     \
};                                                                                                                                    \
                                                                                                                                      \
FuncName##Class<decltype(Substitute)> m_##FuncName##Class;                                                                            \
mutable int m_##FuncName##CallCounter = 0;                                                                                            \
                                                                                                                                      \
public:                                                                                                                               \
                                                                                                                                      \
void Inject##FuncName(decltype(Substitute) sub){                                                                                      \
m_##FuncName##Class.m_func = sub;                                                                                                     \
}                                                                                                                                     \
                                                                                                                                      \
int Get##FuncName##CallCounter(){return m_##FuncName##CallCounter;                                                                    \
}

#define FUNCTION(FuncName,ReturnType, Signature, Substitute, .../*signature parameters*/)                                             \
FUNCTION_INJECTION_SET(FuncName, Substitute)                                                                                          \
ReturnType FuncName Signature {                                                                                                       \
m_##FuncName##CallCounter++;                                                                                                          \
return m_##FuncName##Class.m_func(__VA_ARGS__);                                                                                       \
}

#define FUNCTION_OVERLOAD_INJECTION_SET(FuncName, Substitute, overloadedMethodNumber)                                                 \
private:                                                                                                                              \
FuncName##Class<decltype(Substitute)> m_##FuncName##Class##overloadedMethodNumber;                                                    \
mutable int m_##FuncName##overloadedMethodNumber##CallCounter = 0;                                                                    \
                                                                                                                                      \
public:                                                                                                                               \
void Inject##FuncName(decltype(Substitute) sub){                                                                                      \
m_##FuncName##Class##overloadedMethodNumber.m_func = sub;                                                                             \
}                                                                                                                                     \
int Get##FuncName##overloadedMethodNumber##CallCounter(){return m_##FuncName##overloadedMethodNumber##CallCounter;}

#define FUNCTION_OVERLOAD(FuncName,ReturnType, Signature, Substitute, overloadedMethodNumber, .../*signature parameters*/)         \
FUNCTION_OVERLOAD_INJECTION_SET(FuncName, Substitute, overloadedMethodNumber)                                                                  \
ReturnType FuncName Signature {                                                                                                       \
m_##FuncName##overloadedMethodNumber##CallCounter++;                                                                                  \
return m_##FuncName##Class##overloadedMethodNumber.m_func(__VA_ARGS__);                                                               \
}

#define CAT(param1,param2,param3,param4,param5,param6,param7,param8,param9,param10,param11,param12,param13,param14,param15,param16,param17,param18,param19,param20,...) param1##param2##param3##param4##param5##param6##param7##param8##param9##param10##param11##param12##param13##param14##param15##param16##param17##param18##param19##param20
#define CONCAT(...) CAT(__VA_ARGS__,,,,,,,,,,,,,,,,,,,,,,) //https://stackoverflow.com/a/74729170/1149948


#define FUNC_INJECTION_SET(FuncName, Params, Substitute)                                                                              \
private:                                                                                                                              \
                                                                                                                                      \
template<class T>                                                                                                                     \
class CONCAT(FuncName, _, Params) {                                                                                                       \
	public:                                                                                                                           \
		CONCAT(FuncName, _, Params)():m_func((T)Substitute){}                                                                             \
		T m_func;                                                                                                                     \
};                                                                                                                                    \
                                                                                                                                      \
CONCAT(FuncName, _, Params)<decltype(Substitute)> CONCAT(m_, FuncName, _,Params);                                                            \
mutable int CONCAT(m_, FuncName, CallCounter, _, Params) = 0;                                                                            \
                                                                                                                                      \
public:                                                                                                                               \
                                                                                                                                      \
void CONCAT(Inject, FuncName, _, Params)(decltype(Substitute) sub){                                                                      \
CONCAT(m_, FuncName, _,Params).m_func = sub;                                                                                             \
}                                                                                                                                     \
                                                                                                                                      \
int CONCAT(Get, FuncName, CallCounter, _, Params)(){return CONCAT(m_, FuncName, CallCounter, _, Params);                                    \
}


#define FUNC(FuncName,ReturnType, Signature, Substitute, .../*signature parameters*/)                                                 \
FUNC_INJECTION_SET(FuncName, CONCAT(__VA_ARGS__) , Substitute)                                                                        \
ReturnType FuncName Signature {                                                                                                       \
CONCAT(m_, FuncName, CallCounter, _, CONCAT(__VA_ARGS__))++;                                                                             \
return CONCAT(m_, FuncName, _, __VA_ARGS__).m_func(__VA_ARGS__);                                                                         \
}

#define END_MOCK(MockingClass)                                                                                                        \
private:                                                                                                                              \
std::function<void(void)> m_dtor = [](){};                                                                                            \
public:                                                                                                                               \
void InjectDestructor(std::function<void(void)> dtor) { m_dtor = dtor; }                                                              \
~MockingClass(){m_dtor();}                                                                                                            \
};

#endif // !MOCKINGBIRD
