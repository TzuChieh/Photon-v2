#pragma once

#include "SDL/Introspect/SdlNativeData.h"
#include "Common/assertion.h"
#include "SDL/sdl_traits.h"

#include <utility>

namespace ph
{

template<typename ElementType>
inline SdlNativeData SdlNativeData::fromSingleElement(
	ElementType* const elementPtr,
	const bool canSet,
	const bool canDirectAccess)
{
	PH_ASSERT(elementPtr);

	SdlNativeData nativeData;

	// Note that do not capture element value directly to save memory--value needs to be obtained
	// from pointer, so any data modifications can be observed
	nativeData.m_elementGetter = [elementPtr](std::size_t /* elementIdx */) -> GetterVariant
	{
		return permissiveElementToGetterVariant(elementPtr);
	};

	if(canSet)
	{
		nativeData.m_elementSetter = [elementPtr](std::size_t /* elementIdx */, SetterVariant input) -> bool
		{
			return permissiveSetterVariantToElement(input, elementPtr);
		};
	}

	if(canDirectAccess)
	{
		nativeData.m_directPtr = elementPtr;
	}

	nativeData.numElements = 1;
	return nativeData;
}

inline SdlNativeData::SdlNativeData()
	: SdlNativeData(
		[](std::size_t /* elementIdx */) -> GetterVariant
		{
			return std::monostate{};
		})
{}

inline SdlNativeData::SdlNativeData(ElementGetter getter)
	: SdlNativeData(
		std::move(getter),
		[](std::size_t /* elementIdx */, SetterVariant /* input */) -> bool
		{
			return false;
		})
{}

inline SdlNativeData::SdlNativeData(ElementGetter getter, ElementSetter setter)
	: SdlNativeData(
		std::move(getter), 
		std::move(setter),
		nullptr)
{}

inline SdlNativeData::SdlNativeData(
	ElementGetter getter,
	ElementSetter setter,
	AnyNonConstPtr directPtr)

	: m_elementGetter(std::move(getter))
	, m_elementSetter(std::move(setter))
	, m_directPtr(directPtr)
{}

template<typename T> requires std::is_arithmetic_v<T> || std::is_enum_v<T>
inline std::optional<T> SdlNativeData::get(const std::size_t elementIdx) const
{
	PH_ASSERT_LT(elementIdx, numElements);
	PH_ASSERT(m_elementGetter);

	GetterVariant output = m_elementGetter(elementIdx);
	switch(output.index())
	{
	case 1: return static_cast<T>(std::get<int64>(output));
	case 2: return static_cast<T>(std::get<float64>(output));
	default: return std::nullopt;
	}
}

template<typename T> requires std::is_pointer_v<T>
inline T SdlNativeData::get(const std::size_t elementIdx) const
{
	PH_ASSERT_LT(elementIdx, numElements);
	PH_ASSERT(m_elementGetter);

	using PtrRemovedT = std::remove_pointer_t<T>;

	GetterVariant output = m_elementGetter(elementIdx);
	if constexpr(CSdlInstance<PtrRemovedT>)
	{
		return std::holds_alternative<SdlConstInstance>(output)
			? std::get<SdlConstInstance>(output).get<PtrRemovedT>() : nullptr;
	}
	else
	{
		return std::holds_alternative<AnyConstPtr>(output)
			? std::get<AnyConstPtr>(output).get<PtrRemovedT>() : nullptr;
	}
}

template<typename T> requires std::is_arithmetic_v<T> || std::is_enum_v<T>
inline bool SdlNativeData::set(const std::size_t elementIdx, const T value) const
{
	PH_ASSERT_LT(elementIdx, numElements);
	PH_ASSERT(m_elementSetter);

	if constexpr(std::is_integral_v<T> || std::is_enum_v<T>)
	{
		return m_elementSetter(elementIdx, static_cast<int64>(value));
	}
	else if constexpr(std::is_floating_point_v<T>)
	{
		return m_elementSetter(elementIdx, static_cast<float64>(value));
	}
	else
	{
		return false;
	}
}

template<typename T> requires std::is_pointer_v<T>
inline bool SdlNativeData::set(const std::size_t elementIdx, T const ptr) const
{
	PH_ASSERT_LT(elementIdx, numElements);
	PH_ASSERT(m_elementSetter);

	using PtrRemovedT = std::remove_pointer_t<T>;

	if constexpr(CSdlInstance<PtrRemovedT>)
	{
		return m_elementSetter(elementIdx, SdlNonConstInstance(ptr));
	}
	else
	{
		return m_elementSetter(elementIdx, AnyNonConstPtr(ptr));
	}
}

template<typename T>
inline T* SdlNativeData::directAccess() const
{
	return m_directPtr.get<T>();
}

inline void SdlNativeData::setDirectAccessor(AnyNonConstPtr accessor)
{
	m_directPtr = accessor;
}

inline SdlNativeData::operator bool () const
{
	return numElements > 0;
}

template<typename ElementType>
inline auto SdlNativeData::permissiveElementToGetterVariant(ElementType* const elementPtr)
-> GetterVariant
{
	PH_ASSERT(elementPtr);

	if constexpr(std::is_arithmetic_v<ElementType> || std::is_enum_v<ElementType>)
	{
		const auto elementValue = *elementPtr;
		if constexpr(std::is_integral_v<ElementType> || std::is_enum_v<ElementType>)
		{
			// `int64` may not be able to hold all values (e.g., `uint64` or large enum types), 
			// we `static_cast` anyway since user should instead use direct access if this from of
			// data exchange is unsatisfactory
			return static_cast<int64>(elementValue);
		}
		else if constexpr(std::is_floating_point_v<ElementType>)
		{
			// `float64` should be able to hold any floating point value
			return static_cast<float64>(elementValue);
		}
		else
		{
			PH_STATIC_ASSERT_DEPENDENT_FALSE(ElementType,
				"Unsupported element value type.");
		}
	}
	else if constexpr(CSdlInstance<ElementType>)
	{
		return SdlConstInstance(elementPtr);
	}
	else
	{
		return AnyConstPtr(elementPtr);
	}
}

template<typename ElementType>
inline auto SdlNativeData::permissiveSetterVariantToElement(SetterVariant input, ElementType* const out_elementPtr)
-> bool
{
	PH_ASSERT(out_elementPtr);

	if constexpr(std::is_arithmetic_v<ElementType> || std::is_enum_v<ElementType>)
	{
		// `ElementType` may not be able to hold all values passed in via `input` (e.g., `ElementType`
		// being `char`), we `static_cast` anyway since user should instead use direct access if this 
		// from of data exchange is unsatisfactory
		switch(input.index())
		{
		case 1: 
			*out_elementPtr = static_cast<ElementType>(std::get<int64>(input));
			return true;

		case 2: 
			*out_elementPtr = static_cast<ElementType>(std::get<float64>(input));
			return true;

		default:
			return false;
		}
	}
	else if constexpr(CSdlInstance<ElementType>)
	{
		ElementType* const inputPtr = std::holds_alternative<SdlNonConstInstance>(input)
			? std::get<SdlNonConstInstance>(input).get<ElementType>() : nullptr;
		if(inputPtr)
		{
			*out_elementPtr = *inputPtr;
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		ElementType* const inputPtr = std::holds_alternative<AnyNonConstPtr>(input)
			? std::get<AnyNonConstPtr>(input).get<ElementType>() : nullptr;
		if(inputPtr)
		{
			*out_elementPtr = *inputPtr;
			return true;
		}
		else
		{
			return false;
		}
	}
}

}// end namespace ph
