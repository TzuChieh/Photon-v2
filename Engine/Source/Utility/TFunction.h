#pragma once

#include "Common/config.h"
#include "Common/assertion.h"
#include "Utility/exception.h"

#include <cstddef>
#include <type_traits>
#include <utility>

namespace ph
{

namespace function_detail
{

template<typename T, std::size_t N = 0>
class TFunction final
{
	// Correct function signature will instantiate the specialized type. If this type is selected
	// instead, notify the user about the ill-formed function signature
	static_assert(std::is_function_v<T>,
		"Invalid function signature.");
};

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
	std::is_empty_v<std::decay_t<Func>> &&// so we do not need to store its states
	std::is_default_constructible_v<std::decay_t<Func>> &&// we construct it on every call
	std::is_invocable_r_v<R, Func, Args...>;

template<typename Func, typename R, typename... Args>
concept CNonEmptyTrivialFunctorForm = 
	!std::is_empty_v<std::decay_t<Func>> &&// this also helps to disambiguate from the empty form
	std::is_constructible_v<std::decay_t<Func>, Func> &&// we placement new using existing instance
	std::is_trivially_copyable_v<std::decay_t<Func>> &&// so copying the underlying buffer is legal
	std::is_trivially_destructible_v<std::decay_t<Func>> &&// somewhat redundant as we have `is_trivially_copyable`, but nice to be explicit
	std::is_invocable_r_v<R, Func, Args...>;

/*! @brief Lightweight callable target wrapper.
This type is a thin wrapper around stateless callable targets such as free function, method,
functor and lambda. For methods, the instance must outlive the associated `TFunction`. For functors
and lambdas, they must be stateless (no member variable/no capture) or small enough in size. Unlike
the standard `std::function` which offers little guarantees, this type is guaranteed to be cheap to 
construct, copy, destruct and small in size (minimum: 2 pointers for `MIN_SIZE` == 16, may vary
depending on the platform). Calling functions indirectly through this type adds almost no overhead.

See the amazing post by @bitwizeshift and his github projects, from which is much of the inspiration
derived: https://bitwizeshift.github.io/posts/2021/02/24/creating-a-fast-and-efficient-delegate-type-part-1/
*/
template<typename R, typename... Args, std::size_t MIN_SIZE>
class TFunction<R(Args...), MIN_SIZE> final
{
private:
	using UnifiedCaller = R(*)(const void*, Args...);

	// Aligning to the pointer size should be sufficient in most cases. Currently we do not align the
	// buffer to `std::max_align_t` or anything greater to save space.
	constexpr static std::size_t BUFFER_ALIGNMENT = alignof(void*);

	constexpr static std::size_t BUFFER_SIZE = MIN_SIZE > sizeof(UnifiedCaller) + sizeof(void*)
		? MIN_SIZE - sizeof(UnifiedCaller)
		: sizeof(void*);

	template<typename Func>
	using TCanFitBuffer = std::bool_constant<
		sizeof(Func) <= BUFFER_SIZE &&
		alignof(Func) <= BUFFER_ALIGNMENT>;

public:
	/*! @brief Callable target traits.
	Test whether the target is of specific type and is invocable using @p Args and returns @p R.
	*/
	///@{

	/*! @brief Test if the target @p Func is a free function.
	*/
	template<auto Func>
	using TIsFreeFunction = std::bool_constant<CFreeFunctionForm<Func, R, Args...>>;

	/*! @brief Test if the target @p Func is a method and is invocable with a const object.
	Note that the test is for whether the method is invocable using a const object for the `this`
	argument, not for the constness of the method. See `TIsNonConstCallableMethod` for more examples.
	*/
	template<auto Func, typename Class>
	using TIsConstCallableMethod = std::bool_constant<CConstCallableMethodForm<Func, Class, R, Args...>>;

	/*! @brief Test if the target @p Func is a method and is invocable with a non-const object.
	Note that the test is for whether the method is invocable using a non-const object for the `this`
	argument, not for the constness of the method. For example, a const method is invocable with 
	a non-const instance of @p Class; a method is also invocable using a derived type of @p Class.
	*/
	template<auto Func, typename Class>
	using TIsNonConstCallableMethod = std::bool_constant<CNonConstCallableMethodForm<Func, Class, R, Args...>>;

	/*! @brief Check if the type @p Func is a functor type without member variable.
	The type @p Func must also be default-constructible as we will construct the type on every
	call. Note that a lambda without any capture satisfies this test (it is default-constructible
	since C++20).
	*/
	template<typename Func>
	using TIsEmptyFunctor = std::bool_constant<CEmptyFunctorForm<Func, R, Args...>>;

	template<typename Func>
	using TIsNonEmptyTrivialFunctor = std::bool_constant<
		CNonEmptyTrivialFunctorForm<Func, R, Args...> &&
		TCanFitBuffer<std::decay_t<Func>>>;

	///@}

public:
	/*! @brief Creates an invalid function that cannot be called.
	*/
	inline TFunction() = default;

	inline TFunction(const TFunction& other) = default;
	inline TFunction& operator = (const TFunction& rhs) = default;
	inline ~TFunction() = default;

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
		// Under the assumption that a stateless functor should be cheap to create (and without any
		// side effects), we construct a new `Func` on every call to it
		return Func{}(std::forward<Args>(args)...);
	}

	[[noreturn]]
	inline static R makeInvalidFunctionCaller(const void* /* unused */, Args... args)
	{
		throw UninitializedObjectException("Invalid function call: function is not set");
	}

private:
	struct EmptyStruct
	{};

	union
	{
		// Intentionally provided so that default init of the union is a no-op.
		EmptyStruct u_emptyStruct;

		// Pointer to class instance. May be empty except for methods.
		const void* m_instance = nullptr;

		// Buffer for non-empty functors.
		alignas(BUFFER_ALIGNMENT) std::byte u_buffer[BUFFER_SIZE];
	};

	// Wrapper function with unified signature for calling the actual function.
	UnifiedCaller m_caller = &makeInvalidFunctionCaller;
};

}// end namespace function_detail

template<typename Func, std::size_t MIN_SIZE = PH_TFUNCTION_MIN_SIZE_IN_BYTES>
using TFunction = function_detail::TFunction<Func, MIN_SIZE>;

}// end namespace ph
