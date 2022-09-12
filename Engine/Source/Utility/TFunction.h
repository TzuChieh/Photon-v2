#pragma once

#include "Common/assertion.h"
#include "Utility/exception.h"

#include <type_traits>
#include <utility>

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

template<auto Func, typename R, typename... Args>
concept CFreeFunctionForm = 
	std::is_function_v<std::remove_pointer_t<decltype(Func)>> &&
	std::is_invocable_r_v<R, decltype(Func), Args...>;

template<auto Func, typename Class, typename R, typename... Args>
concept CConstCallableMethodForm = 
	std::is_member_function_pointer_v<decltype(Func)> &&
	std::is_invocable_r_v<R, decltype(Func), const Class*, Args...>;

template<auto Func, typename Class, typename R, typename... Args>
concept CNonConstCallableMethodForm =
	std::is_member_function_pointer_v<decltype(Func)> &&
	std::is_invocable_r_v<R, decltype(Func), Class*, Args...>;

}// end namespace detail

template<typename R, typename... Args>
class TFunction<R(Args...)> final
{
public:
	template<auto Func>
	using TIsFreeFunction = std::bool_constant<detail::CFreeFunctionForm<Func, R, Args...>>;

	template<auto Func, typename Class>
	using TIsConstCallableMethod = std::bool_constant<detail::CConstCallableMethodForm<Func, Class, R, Args...>>;

	template<auto Func, typename Class>
	using TIsNonConstCallableMethod = std::bool_constant<detail::CNonConstCallableMethodForm<Func, Class, R, Args...>>;

public:
	/*! @brief Creates an invalid function that cannot be called.
	*/
	inline TFunction() = default;

	inline TFunction(const TFunction& other) = default;
	inline TFunction& operator = (const TFunction& rhs) = default;

	/*! @brief Call the stored function.
	@exception UninitializedObjectException If the function was invalid (e.g., empty).
	*/
	template<typename... DeducedArgs>
	inline R operator () (DeducedArgs&&... args) const
		requires std::is_invocable_v<R(Args...), DeducedArgs...>
	{
		return (*m_caller)(m_instance, std::forward<DeducedArgs>(args)...);
	}

	template<auto Func>
	inline TFunction& set()
		requires TIsFreeFunction<Func>::value
	{
		m_instance = nullptr;
		m_caller   = &makeFreeFunctionCaller<Func>;

		return *this;
	}

	template<auto Func, typename Class>
		requires TIsConstCallableMethod<Func, Class>::value
	inline TFunction& set(const Class* const instancePtr)
	{
		PH_ASSERT(instancePtr);

		m_instance = instancePtr;
		m_caller   = &makeConstCallableMethodCaller<Func, Class>;

		return *this;
	}

	template<auto Func, typename Class>
		requires TIsNonConstCallableMethod<Func, Class>::value
	inline TFunction& set(Class* const instancePtr)
	{
		PH_ASSERT(instancePtr);

		m_instance = instancePtr;
		m_caller   = &makeNonConstCallableMethodCaller<Func, Class>;

		return *this;
	}

	/*! @brief Check if this function can be called.
	*/
	inline bool isValid() const
	{
		return m_caller != &makeInvalidFunctionCaller;
	}

	/*! @brief Same as isValid().
	*/
	inline operator bool () const
	{
		return isValid();
	}

	/*! @brief Clear the stored function.
	The function becomes invalid after this call.
	*/
	inline void unset()
	{
		m_caller = &makeInvalidFunctionCaller;
	}

private:
	template<auto Func>
	inline static R makeFreeFunctionCaller(const void* /* unused */, Args... args)
		requires TIsFreeFunction<Func>::value
	{
		return (*Func)(std::forward<Args>(args)...);
	}

	template<auto Func, typename Class>
	inline static R makeConstCallableMethodCaller(const void* const rawInstancePtr, Args... args)
		requires TIsConstCallableMethod<Func, Class>::value
	{
		const auto* const instancePtr = static_cast<const Class*>(rawInstancePtr);
		return (instancePtr->*Func)(std::forward<Args>(args)...);
	}

	template<auto Func, typename Class>
	inline static R makeNonConstCallableMethodCaller(const void* const rawInstancePtr, Args... args)
		requires TIsNonConstCallableMethod<Func, Class>::value
	{
		auto* const instancePtr = const_cast<Class*>(static_cast<const Class*>(rawInstancePtr));
		return (instancePtr->*Func)(std::forward<Args>(args)...);
	}

	[[noreturn]]
	inline static R makeInvalidFunctionCaller(const void* /* unused */, Args... args)
	{
		throw UninitializedObjectException("Invalid function call: function is not set");
	}

private:
	using UnifiedCaller = R(*)(const void*, Args...);

	// Wrapper function with unified signature for calling the actual function.
	UnifiedCaller m_caller = &makeInvalidFunctionCaller;

	// Pointer to class instance. May be empty except for methods.
	const void* m_instance = nullptr;
};

}// end namespace ph
