#pragma once

#include <Common/assertion.h>
#include <Common/config.h>
#include <Common/exceptions.h>
#include <Common/memory.h>

#include <cstddef>
#include <type_traits>
#include <utility>
#include <new>
#include <memory>
#include <algorithm>

namespace ph
{

namespace function_detail
{

template<typename T, std::size_t MIN_SIZE_HINT = 0>
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
	std::is_invocable_r_v<R, std::decay_t<Func>, Args...>;// we call from the newly constructed value, no matter the constness

template<typename Func, typename R, typename... Args>
concept CNonEmptyFunctorForm = 
	!std::is_empty_v<std::decay_t<Func>> &&// to disambiguate from the empty form
	!std::is_function_v<std::remove_pointer_t<std::decay_t<Func>>> &&// to disambiguate from the free function form
	(
		std::is_constructible_v<std::decay_t<Func>, Func&&> ||// we can placement new from an instance
		std::is_trivially_copyable_v<std::decay_t<Func>>      // or copy to a byte buffer
	) &&
	std::is_trivially_destructible_v<std::decay_t<Func>> &&// we are neither storing dtor nor calling it
	std::is_invocable_r_v<R, const std::decay_t<Func>&, Args...>;// must be const as we store its states and `operator ()` is `const`

/*! @brief Lightweight callable target wrapper.

This type is a thin wrapper around stateless callable targets such as free function, method,
functor and lambda. For methods, the instance must outlive the associated `TFunction`. For functors
and lambdas, they must be stateless (no member variable/no capture) or small enough in size. Unlike
the standard `std::function` which offers little guarantees, this type is guaranteed to be cheap to 
construct, copy, destruct and small in size (minimum: 2 pointers for `MIN_SIZE_HINT` == 16, may vary
depending on the platform). Calling functions indirectly through this type adds almost no overhead.

@tparam MIN_SIZE_HINT Hint to the minimum size of TFunction. Larger size may correspond to bigger 
internal buffer. A size of 0 will result in the most compact TFunction possible. This is the 
lower-bound of the size of TFunction, and the actual size cannot be guaranteed. Normally, most
supported callable target types will be guaranteed to be storable regardless of this parameter, 
except for non-empty functors.

See the amazing post by @bitwizeshift and his github projects, from which is much of the inspiration
derived: https://bitwizeshift.github.io/posts/2021/02/24/creating-a-fast-and-efficient-delegate-type-part-1/
*/
template<typename R, typename... Args, std::size_t MIN_SIZE_HINT>
class TFunction<R(Args...), MIN_SIZE_HINT> final
{
private:
	using UnifiedCaller = R(*)(const TFunction*, Args...);

	// Aligning to the pointer size should be sufficient in most cases. Currently we do not align the
	// buffer to `std::max_align_t` or anything greater to save space.
	constexpr static std::size_t BUFFER_ALIGNMENT = alignof(void*);

	constexpr static std::size_t BUFFER_SIZE = MIN_SIZE_HINT > sizeof(UnifiedCaller) + sizeof(void*)
		? MIN_SIZE_HINT - sizeof(UnifiedCaller)
		: sizeof(void*);

public:
	/*! @brief Check whether the type @p Func is small enough to be stored in the internal buffer.
	*/
	template<typename Func>
	using TCanFitBuffer = std::bool_constant<
		sizeof(std::decay_t<Func>) <= BUFFER_SIZE &&
		alignof(std::decay_t<Func>) <= BUFFER_ALIGNMENT>;

	/*! @brief Main callable target traits.
	Test whether the target is of specific form and is invocable using @p Args and returns @p R.
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

	/*! @brief Check if the type @p Func is a functor with member variable and can be stored internally.
	The type @p Func must satisfy a series of conditions in order to be safely stored in the internal
	buffer. For specific requirements on the type, see the definition of `CNonEmptyFunctorForm`.
	*/
	template<typename Func>
	using TIsNonEmptyFunctor = std::bool_constant<
		CNonEmptyFunctorForm<Func, R, Args...> &&
		(
			TCanFitBuffer<Func>::value ||            // for placement new into the buffer
			sizeof(std::decay_t<Func>) <= BUFFER_SIZE// for bytewise copy
		)>;

	///@}

	/*! @brief Convenient helper that checks whether @p Func is a supported functor form.
	*/
	template<typename Func>
	using TIsStorableFunctor = std::bool_constant<
		TIsEmptyFunctor<Func>::value ||
		TIsNonEmptyFunctor<Func>::value>;

public:
	/*! @brief Creates an invalid function that cannot be called.
	*/
	inline TFunction() = default;

	/*! @brief Creates an invalid function that cannot be called.
	*/
	inline TFunction(std::nullptr_t /* ptr */)
		: TFunction()
	{}

	/*! @brief Creates a function from functors (including lambdas).
	*/
	template<typename Func>
	inline TFunction(Func&& func)
		requires (!std::is_same_v<std::decay_t<Func>, TFunction>)// avoid ambiguity during copy init
		: TFunction()
	{
		if constexpr(TIsEmptyFunctor<Func>::value)
		{
			set<Func>();
		}
		else
		{
			static_assert(TIsNonEmptyFunctor<Func>::value,
				"Cannot direct-init TFunction with the input functor. Possible cause of errors: "
				"(1) sizeof functor exceeds current limit, reduce functor size or increase the limit; "
				"(2) Invalid/mismatched functor signature; "
				"(3) The direct-init ctor only works for functors. For other function types, please use setters.");

			set<Func>(std::forward<Func>(func));
		}
	}

	inline TFunction(const TFunction& other) = default;
	inline TFunction(TFunction&& other) noexcept = default;
	inline TFunction& operator = (const TFunction& rhs) = default;
	inline TFunction& operator = (TFunction&& rhs) noexcept = default;
	inline ~TFunction() = default;

	/*! @brief Call the stored function.
	@exception UninitializedObjectException If the function was invalid (e.g., empty).
	*/
	template<typename... DeducedArgs>
	inline R operator () (DeducedArgs&&... args) const
		requires std::is_invocable_v<R(Args...), DeducedArgs...>
	{
		return (*m_caller)(this, std::forward<DeducedArgs>(args)...);
	}

	/*! @brief Set a free function.
	*/
	template<auto Func>
	inline TFunction& set()
		requires TIsFreeFunction<Func>::value
	{
		m_data.u_emptyStruct = EmptyStruct{};
		m_caller = &freeFunctionCaller<Func>;

		return *this;
	}

	/*! @brief Set a method callable using a const instance.
	*/
	template<auto Func, typename Class>
	inline TFunction& set(const Class* const instancePtr)
		requires TIsConstCallableMethod<Func, Class>::value
	{
		PH_ASSERT(instancePtr);

		m_data.u_constInstance = instancePtr;
		m_caller = &constCallableMethodCaller<Func, Class>;

		return *this;
	}

	/*! @brief Set a method callable using a non-const instance.
	*/
	template<auto Func, typename Class>
	inline TFunction& set(Class* const instancePtr)
		requires TIsNonConstCallableMethod<Func, Class>::value
	{
		PH_ASSERT(instancePtr);

		m_data.u_nonConstInstance = instancePtr;
		m_caller = &nonConstCallableMethodCaller<Func, Class>;

		return *this;
	}

	/*! @brief Set an empty functor or lambda without capture.
	*/
	template<typename Func>
	inline TFunction& set()
		requires TIsEmptyFunctor<Func>::value
	{
		m_data.u_emptyStruct = EmptyStruct{};
		m_caller = &emptyFunctorCaller<std::decay_t<Func>>;

		return *this;
	}

	/*! @brief Set an empty functor or lambda without capture from object.
	*/
	template<typename Func>
	inline TFunction& set(const Func& /* unused */)
		requires TIsEmptyFunctor<Func>::value
	{
		return set<Func>();
	}

	/*! @brief Set and store a functor or lambda with captures.
	*/
	template<typename Func>
	inline TFunction& set(Func&& func)
		requires TIsNonEmptyFunctor<Func>::value
	{
		using Functor = std::decay_t<Func>;

		// Favor constructed functor since it is more efficient in general
		if constexpr(std::is_constructible_v<Functor, Func&&>)
		{
			// IOC of array of size 1
			Functor* const storage = start_implicit_lifetime_as<Functor>(m_data.u_buffer);

			std::construct_at(storage, std::forward<Func>(func));
			m_caller = &nonEmptyConstructedFunctorCaller<Functor>;
		}
		else
		{
			static_assert(std::is_trivially_copyable_v<Functor>);

			std::copy_n(reinterpret_cast<const std::byte*>(&func), sizeof(Functor), m_data.u_buffer);
			m_caller = &nonEmptyCopiedFunctorCaller<Functor>();
		}
		
		return *this;
	}

	/*! @brief Check if this function can be called.
	*/
	inline bool isValid() const
	{
		return m_caller != &invalidFunctionCaller;
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
		m_caller = &invalidFunctionCaller;
	}

private:
	template<auto Func>
	inline static R freeFunctionCaller(const TFunction* /* unused */, Args... args)
		requires TIsFreeFunction<Func>::value
	{
		return (*Func)(std::forward<Args>(args)...);
	}

	template<auto Func, typename Class>
	inline static R constCallableMethodCaller(const TFunction* const self, Args... args)
		requires TIsConstCallableMethod<Func, Class>::value
	{
		const auto* const instancePtr = static_cast<const Class*>(self->m_data.u_constInstance);
		return (instancePtr->*Func)(std::forward<Args>(args)...);
	}

	template<auto Func, typename Class>
	inline static R nonConstCallableMethodCaller(const TFunction* const self, Args... args)
		requires TIsNonConstCallableMethod<Func, Class>::value
	{
		auto* const instancePtr = static_cast<Class*>(self->m_data.u_nonConstInstance);
		
		return (instancePtr->*Func)(std::forward<Args>(args)...);
	}

	template<typename Func>
	inline static R emptyFunctorCaller(const TFunction* /* unused */, Args... args)
		requires TIsEmptyFunctor<Func>::value
	{
		// Under the assumption that a stateless functor should be cheap to create (and without any
		// side effects), we construct a new `Func` on every call to it
		return Func{}(std::forward<Args>(args)...);
	}

	template<typename Func>
	inline static R nonEmptyConstructedFunctorCaller(const TFunction* const self, Args... args)
	{
		// We do not obtain the pointer to `Func` via placement new (or `std::construct_at`).
		// Instead, we cast it from raw buffer and laundering is required by the standard
		const auto& func = *std::launder(reinterpret_cast<const Func*>(self->m_data.u_buffer));

		return func(std::forward<Args>(args)...);
	}

	template<typename Func>
	inline static R nonEmptyCopiedFunctorCaller(const TFunction* const self, Args... args)
	{
		static_assert(std::is_trivially_copyable_v<Func> && std::is_default_constructible_v<Func>,
			"Current implementation of copied functor caller requires default constructible functor "
			"to copy bytes into.");

		Func func;
		std::copy_n(self->m_data.u_buffer, sizeof(Func), reinterpret_cast<std::byte*>(&func));

		return func(std::forward<Args>(args)...);
	}

	[[noreturn]]
	inline static R invalidFunctionCaller(const TFunction* /* unused */, Args... args)
	{
		throw UninitializedObjectException("Invalid function call: function is not set");
	}

private:
	struct EmptyStruct
	{};

	union Data
	{
		// Intentionally provided so that default init of the union is a no-op.
		EmptyStruct u_emptyStruct;

		// Pointer to const class instance. May be empty except for methods.
		const void* u_constInstance;

		// Pointer to non-const class instance. May be empty except for methods.
		void* u_nonConstInstance;

		// Buffer for non-empty functors.
		alignas(BUFFER_ALIGNMENT) std::byte u_buffer[BUFFER_SIZE];
	};

	// Ensure we are not wasting memory. Adjust buffer alignment if failed.
	static_assert(alignof(Data) == BUFFER_ALIGNMENT);

	// Member variables: smallest possible size of `TFunction` is two pointers--one for `UnifiedCaller` 
	// and another one in `Data`

	Data m_data = {EmptyStruct{}};

	// Wrapper function with unified signature for calling the actual function.
	UnifiedCaller m_caller = &invalidFunctionCaller;
};

}// end namespace function_detail

template<typename Func, std::size_t MIN_SIZE_HINT = PH_TFUNCTION_DEFAULT_MIN_SIZE_IN_BYTES>
using TFunction = function_detail::TFunction<Func, MIN_SIZE_HINT>;

// This is a stricter requirement than what `TFunction` guaranteed. However, if its code works 
// correctly the size should be exactly what has been requested (providing the hint is >= 16 bytes).
static_assert(sizeof(TFunction<int(int, int)>) == PH_TFUNCTION_DEFAULT_MIN_SIZE_IN_BYTES);

}// end namespace ph
