#pragma once

#include "Common/assertion.h"

#include <type_traits>

namespace ph
{

template<typename T>
class TFunction final
{
	// Correct function signature will instantiate the specialized type. If this type is selected
	// instead, notify the user about the ill-formed function signature
	static_assert(std::is_function_v<T>,
		"Invalid function signature.");
};

namespace detail
{

template<auto FreeFunction, typename R, typename... Args>
concept CFreeFunctionForm = 
	std::is_function_v<decltype(FreeFunction)> && 
	std::is_invocable_r_v<R, decltype(FreeFunction), Args...>;

template<auto ConstCallableMethod, typename Class, typename R, typename... Args>
concept CConstMethodForm = 
	std::is_member_function_pointer_v<decltype(ConstCallableMethod)> &&
	std::is_invocable_r_v<R, decltype(ConstCallableMethod), const Class*, Args...>;

template<auto NonConstCallableMethod, typename Class, typename R, typename... Args>
concept CNonConstMethodForm = 
	std::is_member_function_pointer_v<decltype(NonConstCallableMethod)> &&
	std::is_invocable_r_v<R, decltype(NonConstCallableMethod), Class*, Args...>;

}// end namespace detail

template<typename R, typename... Args>
class TFunction<R(Args...)> final
{
public:
	template<auto FreeFunction>
	using TIsFreeFunction = std::bool_constant<detail::CFreeFunctionForm<FreeFunction, R, Args...>>;

	template<auto ConstMethod, typename Class>
	using TIsConstMethod = std::bool_constant<detail::CConstMethodForm<ConstMethod, Class, R, Args...>>;

	template<auto NonConstMethod, typename Class>
	using TIsNonConstMethod = std::bool_constant<detail::CNonConstMethodForm<NonConstMethod, Class, R, Args...>>;

public:
	/*! @brief Creates null function that cannot be called.
	*/
	inline TFunction() = default;

	inline TFunction(const TFunction& other) = default;
	inline TFunction& operator = (const TFunction& rhs) = default;

	/*! @brief Call the stored function.
	*/
	inline R operator () (Args... args) const
	{
		// TODO
	}

	template<auto FreeFunction>
	inline void set()
		requires TIsFreeFunction<FreeFunction>{}
	{
		m_instance = nullptr;
		m_caller   = &makeFreeFunctionCaller<FreeFunction>;
	}

	template<auto ConstMethod, typename Class>
		requires TIsConstMethod<ConstMethod, Class>{}
	inline void set(const Class* const instancePtr)
	{
		PH_ASSERT(instancePtr);

		m_instance = instancePtr;
		m_caller   = &makeConstMethodCaller<ConstMethod, Class>;
	}

	template<auto NonConstMethod, typename Class>
		requires TIsNonConstMethod<NonConstMethod, Class>{}
	inline void set(Class* const instancePtr)
	{
		PH_ASSERT(instancePtr);

		m_instance = instancePtr;
		m_caller   = &makeNonConstMethodCaller<NonConstMethod, Class>;
	}

private:
	template<auto FreeFunction>
	inline static R makeFreeFunctionCaller(const void* /* unused */, Args... args)
		requires TIsFreeFunction<FreeFunction>{}
	{
		return (*FreeFunction)(args...);
	}

	template<auto ConstMethod, typename Class>
	inline static R makeConstMethodCaller(const void* const rawInstancePtr, Args... args)
		requires TIsConstMethod<ConstMethod, Class>{}
	{
		const auto* const instancePtr = static_cast<const Class*>(rawInstancePtr);
		return (instancePtr->*ConstMethod)(args...);
	}

	template<auto NonConstMethod, typename Class>
	inline static R makeNonConstMethodCaller(const void* const rawInstancePtr, Args... args)
		requires TIsNonConstMethod<NonConstMethod, Class>{}
	{
		auto* const instancePtr = const_cast<Class*>(static_cast<const Class*>(rawInstancePtr));
		return (instancePtr->*NonConstMethod)(args...);
	}

private:
	using UnifiedCaller = R(*)(const void*, Args...);

	// Wrapper function with unified signature for calling the actual function.
	UnifiedCaller m_caller = nullptr;

	// Pointer to class instance. May be empty except for methods.
	const void* m_instance = nullptr;
};

}// end namespace ph
