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

// TODO: concepts for func sig

}// end namespace detail

template<typename R, typename... Args>
class TFunction<R(Args...)> final
{
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
		requires std::is_function_v<decltype(FreeFunction)>
	{
		m_instance = nullptr;
		m_caller   = &makeFreeFunctionCaller<FreeFunction>;
	}

	template<auto ConstMethod, typename Class>
		requires std::is_member_function_pointer_v<decltype(ConstMethod)>
	inline void set(const Class* const instancePtr)
	{
		PH_ASSERT(instancePtr);

		m_instance = instancePtr;
		m_caller   = &makeConstMethodCaller<ConstMethod, Class>;
	}

private:
	template<auto FreeFunction>
	inline static R makeFreeFunctionCaller(const void* /* unused */, Args... args)
		requires std::is_function_v<decltype(FreeFunction)> && std::is_invocable_r_v<R, decltype(FreeFunction), Args...>
	{
		return (*FreeFunction)(args...);
	}

	template<auto ConstMethod, typename Class>
	inline static R makeConstMethodCaller(const void* const rawInstancePtr, Args... args)
		requires std::is_member_function_pointer_v<decltype(ConstMethod)> && std::is_invocable_r_v<R, decltype(ConstMethod), const Class*, Args...>
	{
		const auto* const instancePtr = static_cast<const Class*>(rawInstancePtr);
		return (instancePtr->*ConstMethod)(args...);
	}

	template<auto NonConstMethod, typename Class>
	inline static R makeNonConstMethodCaller(const void* const rawInstancePtr, Args... args)
		requires std::is_member_function_pointer_v<decltype(NonConstMethod)> && std::is_invocable_r_v<R, decltype(NonConstMethod), Class*, Args...>
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
