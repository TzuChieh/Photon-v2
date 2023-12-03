#pragma once

#include "SDL/TSdlAnyInstance.h"
#include "Common/assertion.h"
#include "Utility/traits.h"
#include "Utility/utility.h"
#include "SDL/sdl_traits.h"
#include "SDL/sdl_helpers.h"
#include "SDL/Introspect/SdlClass.h"
#include "SDL/Introspect/SdlStruct.h"
#include "SDL/Introspect/SdlFunction.h"

#include <utility>

namespace ph
{

template<bool IS_CONST>
inline TSdlAnyInstance<IS_CONST>::TSdlAnyInstance()
	: TSdlAnyInstance(nullptr)
{}

template<bool IS_CONST>
inline TSdlAnyInstance<IS_CONST>::TSdlAnyInstance(std::nullptr_t /* instance */)
	: m_instance()
	, m_meta()
{}

template<bool IS_CONST>
template<typename T>
inline TSdlAnyInstance<IS_CONST>::TSdlAnyInstance(T* const target)
	: TSdlAnyInstance()
{
	static_assert(sizeof(T) == sizeof(T),
		"Input must be a complete type.");
	static_assert(!(!IS_CONST && std::is_const_v<T>),
		"Input instance is const, cannot convert it to non-const.");

	if constexpr(CDerived<T, ISdlResource>)
	{
		// `T` may be const qualified; this automatically sets the right type (void pointer has
		// lower rank in overload resolution)
		m_instance = target;

		if(target)
		{
			m_meta = target->getDynamicSdlClass();
		}
		else if constexpr(CHasSdlClassDefinition<T>)
		{
			m_meta = T::getSdlClass();
		}
	}
	else if constexpr(CHasSdlStructDefinition<T>)
	{
		// `T` may be const qualified; this automatically sets the right type
		m_instance = target;

		m_meta = T::getSdlStruct();
	}
	else if constexpr(CHasSdlFunctionDefinition<T>)
	{
		// `T` may be const qualified; this automatically sets the right type
		m_instance = target;

		m_meta = T::getSdlFunction();
	}
	else
	{
		PH_STATIC_ASSERT_DEPENDENT_FALSE(T,
			"Input is not a valid SDL target type (must be a SDL class/struct/function).");

		// Make sure every invalid `T` also fail this concept
		static_assert(!CSdlInstance<T>);
	}

	// Make sure every valid `T` also satisfy this concept
	static_assert(CSdlInstance<T>);
}

template<bool IS_CONST>
template<typename T>
inline auto* TSdlAnyInstance<IS_CONST>::get() const
{
	using ReturnType = std::conditional_t<IS_CONST, const T, T>;

	// Only one of class, struct and function can exist (may all be null also)
	PH_ASSERT_LE(
		(getClass() != nullptr) + (getStruct() != nullptr) + (getFunction() != nullptr),
		1);

	if constexpr(CDerived<T, ISdlResource>)
	{
		ClassInstanceType* const instance = std::holds_alternative<ClassInstanceType*>(m_instance)
			? std::get<ClassInstanceType*>(m_instance) : nullptr;
		if(instance)
		{
			return sdl::cast_to<ReturnType>(instance);
		}
	}
	else if constexpr(CHasSdlStructDefinition<T>)
	{
		if(std::holds_alternative<StructInstanceType*>(m_instance))
		{
			// Ensure `T` and stored instance are the same type before casting
			if(T::getSdlStruct() == getStruct())
			{
				StructInstanceType* const instance = std::get<StructInstanceType*>(m_instance);
				return static_cast<ReturnType*>(instance);
			}
		}
	}
	else if constexpr(CHasSdlFunctionDefinition<T>)
	{
		if(std::holds_alternative<StructInstanceType*>(m_instance))
		{
			// Ensure `T` and stored instance are the same type before casting
			if(T::getSdlFunction() == getFunction())
			{
				StructInstanceType* const instance = std::get<StructInstanceType*>(m_instance);
				return static_cast<ReturnType*>(instance);
			}
		}
	}

	return static_cast<ReturnType*>(nullptr);
}

template<bool IS_CONST>
template<typename T>
inline const auto* TSdlAnyInstance<IS_CONST>::getConst() const
{
	return std::as_const(get<T>());
}

template<bool IS_CONST>
inline const SdlClass* TSdlAnyInstance<IS_CONST>::getClass() const
{
	return std::holds_alternative<const SdlClass*>(m_meta)
		? std::get<const SdlClass*>(m_meta) : nullptr;
}

template<bool IS_CONST>
inline const SdlStruct* TSdlAnyInstance<IS_CONST>::getStruct() const
{
	return std::holds_alternative<const SdlStruct*>(m_meta)
		? std::get<const SdlStruct*>(m_meta) : nullptr;
}

template<bool IS_CONST>
inline const SdlFunction* TSdlAnyInstance<IS_CONST>::getFunction() const
{
	return std::holds_alternative<const SdlFunction*>(m_meta)
		? std::get<const SdlFunction*>(m_meta) : nullptr;
}

template<bool IS_CONST>
inline bool TSdlAnyInstance<IS_CONST>::isClass() const
{
	return m_meta.index() == variant_index_of<const SdlClass*>(m_meta);
}

template<bool IS_CONST>
inline bool TSdlAnyInstance<IS_CONST>::isStruct() const
{
	return m_meta.index() == variant_index_of<const SdlStruct*>(m_meta);
}

template<bool IS_CONST>
inline bool TSdlAnyInstance<IS_CONST>::isFunction() const
{
	return m_meta.index() == variant_index_of<const SdlFunction*>(m_meta);
}

template<bool IS_CONST>
inline const ISdlInstantiable* TSdlAnyInstance<IS_CONST>::getInstantiable() const
{
	constexpr auto classIdx = variant_index_of<const SdlClass*, MetaType>();
	constexpr auto structIdx = variant_index_of<const SdlStruct*, MetaType>();
	constexpr auto functionIdx = variant_index_of<const SdlFunction*, MetaType>();

	switch(m_meta.index())
	{
	case classIdx: return std::get<classIdx>(m_meta);
	case structIdx: return std::get<structIdx>(m_meta);
	case functionIdx: return std::get<functionIdx>(m_meta);
	default: return nullptr;
	}
}

template<bool IS_CONST>
inline TSdlAnyInstance<IS_CONST>::operator bool () const
{
	constexpr auto classInstanceIdx = variant_index_of<ClassInstanceType*, InstanceType>();
	constexpr auto structInstanceIdx = variant_index_of<StructInstanceType*, InstanceType>();

	switch(m_instance.index())
	{
	case classInstanceIdx: return std::get<classInstanceIdx>(m_instance) != nullptr;
	case structInstanceIdx: return std::get<structInstanceIdx>(m_instance) != nullptr;
	default: return false;
	}
}

}// end namespace ph
