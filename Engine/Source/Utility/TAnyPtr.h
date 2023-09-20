#pragma once

#include <utility>
#include <cstddef>
#include <type_traits>
#include <typeindex>

namespace ph
{

/*! @brief A type-safe, lightweight wrapper for any raw pointer type.
Using `std::any` with a raw pointer type could achieve similar functionality. However, this class 
is dedicated for raw pointers and is guaranteed to have no dynamic allocation.
*/
template<bool IS_CONST>
class TAnyPtr final
{
public:
	TAnyPtr();

	TAnyPtr(std::nullptr_t ptr);

	template<typename T>
	TAnyPtr(T* ptr);

	template<typename T>
	auto* get() const;

	template<typename T>
	const T* getConst() const;

	operator bool () const;

	template<typename T>
	operator T* () const;

private:
	using PointerType = std::conditional_t<IS_CONST, const void*, void*>;

	PointerType m_pointer;
	std::type_index m_pointedType;
};

/*! @brief A type-safe, lightweight wrapper for any const raw pointer type.
*/
using AnyConstPtr = TAnyPtr<true>;

/*! @brief A type-safe, lightweight wrapper for any non-const raw pointer type.
*/
using AnyNonConstPtr = TAnyPtr<false>;

template<bool IS_CONST>
inline TAnyPtr<IS_CONST>::TAnyPtr()
	: TAnyPtr(nullptr)
{}

template<bool IS_CONST>
inline TAnyPtr<IS_CONST>::TAnyPtr(std::nullptr_t /* ptr */)
	: m_pointer(nullptr)
	, m_pointedType(typeid(std::nullptr_t))
{}

template<bool IS_CONST>
template<typename T>
inline TAnyPtr<IS_CONST>::TAnyPtr(T* const ptr)
	: m_pointer(ptr)
	, m_pointedType(typeid(T))
{
	static_assert(sizeof(T) == sizeof(T),
		"Input must be a complete type.");
	static_assert(!(!IS_CONST && std::is_const_v<T>),
		"Input pointer is const, cannot convert it to non-const.");
}

template<bool IS_CONST>
template<typename T>
inline auto* TAnyPtr<IS_CONST>::get() const
{
	// Also handles the case where `const` is explicitly specified in `T`
	using ReturnType = std::conditional_t<IS_CONST, const T, T>;

	if(m_pointedType == typeid(T))
	{
		return static_cast<ReturnType*>(m_pointer);
	}
	else
	{
		return static_cast<ReturnType*>(nullptr);
	}
}

template<bool IS_CONST>
template<typename T>
inline const T* TAnyPtr<IS_CONST>::getConst() const
{
	return std::as_const(get<T>());
}

template<bool IS_CONST>
inline TAnyPtr<IS_CONST>::operator bool () const
{
	return m_pointer != nullptr;
}

template<bool IS_CONST>
template<typename T>
inline TAnyPtr<IS_CONST>::operator T* () const
{
	// Note that we could, of course, write a better return type that matches `IS_CONST`. Here a more
	// general type `T*` is used, so we could detect potential conversion errors here and provide
	// a more friendly error message.
	//
	static_assert(!(IS_CONST && !std::is_const_v<T>),
		"Cannot convert a const pointer to non-const.");

	return get<T>();
}

}// end namespace ph
