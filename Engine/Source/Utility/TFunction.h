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

template<typename Func, typename R, typename... Args>
concept CEmptyFunctorForm = 
	std::is_empty_v<Func> &&
	std::is_default_constructible_v<Func> &&
	std::is_invocable_r_v<R, Func, Args...>;

}// end namespace detail

/*! @brief Lightweight callable target wrapper.
This type is a thin wrapper around stateless callable targets such as free function, method,
functor and lambda. For methods, the instance must outlive the associated `TFunction`. For functors
and lambdas, they must be stateless (no member variable/no capture). This type is guaranteed to
be cheap to construct, copy, destruct and little in size (2 pointers). Calling functions indirectly
through this type adds almost no overhead.

See the amazing post by @bitwizeshift and his github projects, from which is much of the inspiration
derived: https://bitwizeshift.github.io/posts/2021/02/24/creating-a-fast-and-efficient-delegate-type-part-1/
*/
template<typename R, typename... Args>
class TFunction<R(Args...)> final
{
public:
	/*! @brief Callable target traits.
	Test whether the target is of specific type and is invocable using @p Args and returns @p R.
	*/
	///@{

	/*! @brief Test if the target @p Func is a free function.
	*/
	template<auto Func>
	using TIsFreeFunction = std::bool_constant<detail::CFreeFunctionForm<Func, R, Args...>>;

	/*! @brief Test if the target @p Func is a method and is invocable with a const object.
	Note that the test is for whether the method is invocable using a const object for the `this`
	argument, not for the constness of the method. See `TIsNonConstCallableMethod` for more examples.
	*/
	template<auto Func, typename Class>
	using TIsConstCallableMethod = std::bool_constant<detail::CConstCallableMethodForm<Func, Class, R, Args...>>;

	/*! @brief Test if the target @p Func is a method and is invocable with a non-const object.
	Note that the test is for whether the method is invocable using a non-const object for the `this`
	argument, not for the constness of the method. For example, a const method is invocable with 
	a non-const instance of @p Class; a method is also invocable using a derived type of @p Class.
	*/
	template<auto Func, typename Class>
	using TIsNonConstCallableMethod = std::bool_constant<detail::CNonConstCallableMethodForm<Func, Class, R, Args...>>;

	/*! @brief Check if the type @p Func is a functor type without member variable.
	The type @p Func must also be default-constructible as we will construct the type on every
	call. Note that a lambda without any capture satisfies this test (it is default-constructible
	since C++20).
	*/
	template<typename Func>
	using TIsEmptyFunctor = std::bool_constant<detail::CEmptyFunctorForm<Func, R, Args...>>;

	///@}

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

	/*! @brief Set a free function.
	*/
	template<auto Func>
	inline TFunction& set()
		requires TIsFreeFunction<Func>::value
	{
		m_instance = nullptr;
		m_caller   = &makeFreeFunctionCaller<Func>;

		return *this;
	}

	/*! @brief Set a method callable using a const instance.
	*/
	template<auto Func, typename Class>
	inline TFunction& set(const Class* const instancePtr)
		requires TIsConstCallableMethod<Func, Class>::value
	{
		PH_ASSERT(instancePtr);

		m_instance = instancePtr;
		m_caller   = &makeConstCallableMethodCaller<Func, Class>;

		return *this;
	}

	/*! @brief Set a method callable using a non-const instance.
	*/
	template<auto Func, typename Class>
	inline TFunction& set(Class* const instancePtr)
		requires TIsNonConstCallableMethod<Func, Class>::value
	{
		PH_ASSERT(instancePtr);

		m_instance = instancePtr;
		m_caller   = &makeNonConstCallableMethodCaller<Func, Class>;

		return *this;
	}

	/*! @brief Set an empty functor or lambda without capture.
	*/
	template<typename Func>
	inline TFunction& set()
		requires TIsEmptyFunctor<Func>::value
	{
		m_instance = nullptr;
		m_caller   = &makeEmptyFunctorCaller<Func>;

		return *this;
	}

	/*! @brief Set an empty functor or lambda without capture from object.
	*/
	template<typename Func>
	inline TFunction& set(Func /* unused */)
		requires TIsEmptyFunctor<Func>::value
	{
		return set<Func>();
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

	template<typename Func>
	inline static R makeEmptyFunctorCaller(const void* /* unused */, Args... args)
		requires TIsEmptyFunctor<Func>::value
	{
		return Func{}(std::forward<Args>(args)...);
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
