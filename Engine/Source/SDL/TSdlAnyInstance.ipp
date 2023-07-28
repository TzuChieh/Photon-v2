#pragma once

#include "SDL/TSdlAnyInstance.h"
#include "Common/assertion.h"
#include "Utility/traits.h"
#include "SDL/sdl_traits.h"
#include "SDL/sdl_helpers.h"

#include <utility>

namespace ph
{

template<bool IS_CONST>
inline TSdlAnyInstance<IS_CONST>::TSdlAnyInstance()
	: m_instance()
	, m_meta()
{}

template<bool IS_CONST>
template<typename T>
inline TSdlAnyInstance<IS_CONST>::TSdlAnyInstance(T* const instance)
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
		m_instance = instance;

		if(instance)
		{
			m_meta = instance->getDynamicSdlClass();
		}
		else if constexpr(CHasSdlClassDefinition<T>)
		{
			m_meta = T::getSdlClass();
		}
	}
	else if constexpr(CHasSdlStructDefinition<T>)
	{
		// `T` may be const qualified; this automatically sets the right type
		m_instance = instance;

		m_meta = T::getSdlStruct();
	}
	else
	{
		PH_STATIC_ASSERT_DEPENDENT_FALSE(T,
			"Input is not a valid SDL instance type (must be a SDL class/struct).");
	}
}

template<bool IS_CONST>
template<typename T>
inline auto* TSdlAnyInstance<IS_CONST>::get() const
{
	using ReturnType = std::conditional_t<IS_CONST, const T, T>;

	// Only one of class and struct can exist (may both be null also)
	PH_ASSERT(!(getClass() && getStruct()));

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
			// If a struct instance is stored, meta information must be there, too
			// (no matter the instance is null or not)
			PH_ASSERT(std::holds_alternative<const SdlStruct*>(m_meta));

			// Ensure `T` and stored instance are the same type before casting
			if(T::getSdlStruct() == std::get<const SdlStruct*>(m_meta))
			{
				StructInstanceType* const instance = std::get<StructInstanceType*>(m_instance);
				return static_cast<ReturnType*>(instance);
			}
		}
	}
	else
	{
		PH_STATIC_ASSERT_DEPENDENT_FALSE(T,
			"Input is not a valid SDL instance type (must be a SDL class/struct).");
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
inline TSdlAnyInstance<IS_CONST>::operator bool () const
{
	switch(m_instance.index())
	{
	case 1: return std::get<ClassInstanceType*>(m_instance) != nullptr;
	case 2: return std::get<StructInstanceType*>(m_instance) != nullptr;
	default: return false;
	}
}

}// end namespace ph
