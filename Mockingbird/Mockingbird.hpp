/*
 *  Mockingbird is a simple and a powerful mocking framework for c++.
 *  Copyright (c) Mouaz Chamieh April 2021.
 *  Distributed under the MIT License. Please refer to the LICENSE file at:
 *  https://github.com/muazsh/Mockingbird
 */

#pragma once
#ifndef MOCKINGBIRD
#define MOCKINGBIRD

#define START_MOCK(MockingClass, MockedClass)						\
class MockingClass : public MockedClass {							\
 public:

#define INJECTION_SET(FuncName, Substitute)										\
private:																		\
																				\
template<class T>																\
class FuncName##Class {															\
	public:																		\
		FuncName##Class():m_func((T)Substitute){}								\
		T m_func;																\
};																				\
																				\
FuncName##Class<decltype(Substitute)> m_##FuncName##Class;						\
mutable int m_##FuncName##CallCounter = 0;										\
																				\
public:																			\
																				\
void Inject##FuncName(decltype(Substitute) sub){								\
m_##FuncName##Class.m_func = sub;												\
}																				\
																				\
int Get##FuncName##CallCounter(){return m_##FuncName##CallCounter;				\
}

#define FUNCTION(FuncName,ReturnType, Signature, Substitute, .../*signature variables*/)\
INJECTION_SET(FuncName, Substitute)														\
ReturnType FuncName Signature override {												\
m_##FuncName##CallCounter++;															\
return m_##FuncName##Class.m_func(__VA_ARGS__);											\
}

#define CONST_FUNCTION(FuncName,ReturnType, Signature, Substitute, .../*signature variables*/)	\
INJECTION_SET(FuncName, Substitute)																\
ReturnType FuncName Signature const override{													\
m_##FuncName##CallCounter++;																	\
return m_##FuncName##Class.m_func(__VA_ARGS__);													\
}

#define OVERLOAD_INJECTION_SET(FuncName, Substitute, overloadedMethodNumber)				\
private:																					\
FuncName##Class<decltype(Substitute)> m_##FuncName##Class##overloadedMethodNumber;			\
mutable int m_##FuncName##overloadedMethodNumber##CallCounter = 0;							\
																							\
public:																						\
void Inject##FuncName(decltype(Substitute) sub){											\
m_##FuncName##Class##overloadedMethodNumber.m_func = sub;									\
}																							\
int Get##FuncName##overloadedMethodNumber##CallCounter(){return m_##FuncName##CallCounter;}

#define FUNCTION_OVERLOADING(FuncName,ReturnType, Signature, Substitute, overloadedMethodNumber, .../*signature variables*/)\
OVERLOAD_INJECTION_SET(FuncName, Substitute, overloadedMethodNumber)														\
ReturnType FuncName Signature override{																						\
m_##FuncName##overloadedMethodNumber##CallCounter++;																		\
return m_##FuncName##Class##overloadedMethodNumber.m_func(__VA_ARGS__);														\
}

#define CONST_FUNCTION_OVERLOADING(FuncName,ReturnType, Signature, Substitute, overloadedMethodNumber, .../*signature variables*/)	\
OVERLOAD_INJECTION_SET(FuncName, Substitute, overloadedMethodNumber)																\
ReturnType FuncName Signature const override{																						\
m_##FuncName##overloadedMethodNumber##CallCounter++;																				\
return m_##FuncName##Class##overloadedMethodNumber.m_func(__VA_ARGS__);																\
}

#define END_MOCK(MockingClass)\
};

#endif // !MOCKINGBIRD
