#pragma once

#include "SDL/Introspect/SdlNativeData.h"
#include "Common/assertion.h"
#include "SDL/sdl_traits.h"

#include <utility>

namespace ph
{

template<typename Variant>
inline TSdlAccessorVariant<Variant>::TSdlAccessorVariant()
	: m_variant(std::monostate{})
{}

template<typename Variant>
template<typename T>
inline TSdlAccessorVariant<Variant>::TSdlAccessorVariant(T value)
	: m_variant(std::move(value))
{}

template<typename Variant>
inline TSdlAccessorVariant<Variant>::TSdlAccessorVariant(VariantType variant)
	: m_variant(std::move(variant))
{}

template<typename Variant>
template<typename T>
inline bool TSdlAccessorVariant<Variant>::has() const
{
	return std::holds_alternative<T>(m_variant);
}

template<typename Variant>
template<typename T>
inline T& TSdlAccessorVariant<Variant>::get()
{
	PH_ASSERT(has<T>());
	return std::get<T>(m_variant);
}

template<typename Variant>
template<typename T>
inline const T& TSdlAccessorVariant<Variant>::get() const
{
	PH_ASSERT(has<T>());
	return std::get<T>(m_variant);
}

template<typename Variant>
template<typename T>
inline void TSdlAccessorVariant<Variant>::set(T value)
{
	std::get<T>(m_variant) = std::move(value);
}

template<typename Variant>
inline std::size_t TSdlAccessorVariant<Variant>::index() const
{
	return m_variant.index();
}

template<typename Variant>
inline bool TSdlAccessorVariant<Variant>::isEmpty() const
{
	return has<std::monostate>();
}

template<typename Variant>
inline auto TSdlAccessorVariant<Variant>::getVariant()
-> VariantType&
{
	return m_variant;
}

template<typename Variant>
inline auto TSdlAccessorVariant<Variant>::getVariant() const
-> const VariantType&
{
	return m_variant;
}

template<typename ElementType>
inline SdlNativeData SdlNativeData::fromSingleElement(
	ElementType* const elementPtr,
	ESdlDataFormat elementContainer,
	ESdlDataType elementType,
	const bool canSet,
	const bool canDirectAccess)
{
	SdlNativeData nativeData;

	// We do not capture element value directly to save memory--value needs to be obtained from
	// pointer, so any data modifications can be observed.

	if(elementPtr)
	{
		nativeData.m_elementGetter =
			[elementPtr](std::size_t /* elementIdx */) -> SdlGetterVariant
			{
				return permissiveElementGetter(elementPtr);
			};
	}
	else
	{
		nativeData.m_elementGetter =
			[elementPtr](std::size_t /* elementIdx */) -> SdlGetterVariant
			{
				return std::monostate{};
			};
	}
	
	if(canSet)
	{
		if(elementPtr)
		{
			nativeData.m_elementSetter =
				[elementPtr](std::size_t /* elementIdx */, SdlSetterVariant input) -> bool
				{
					return permissiveElementSetter(input, elementPtr);
				};
		}
		else
		{
			nativeData.m_elementSetter =
				[elementPtr](std::size_t /* elementIdx */, SdlSetterVariant input) -> bool
				{
					return false;
				};
		}
	}

	if(canDirectAccess)
	{
		nativeData.m_directPtr = elementPtr;
	}

	nativeData.numElements = elementPtr ? 1 : 0;
	nativeData.elementContainer = elementContainer;
	nativeData.elementType = elementType;
	return nativeData;
}

inline SdlNativeData::SdlNativeData()
	: SdlNativeData(
		[](std::size_t /* elementIdx */) -> SdlGetterVariant
		{
			return std::monostate{};
		})
{}

inline SdlNativeData::SdlNativeData(ElementGetter getter)
	: SdlNativeData(
		std::move(getter),
		[](std::size_t /* elementIdx */, SdlSetterVariant /* input */) -> bool
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
	PH_ASSERT(m_elementGetter);

	SdlGetterVariant output = m_elementGetter(elementIdx);
	switch(output.index())
	{
	case 1: return static_cast<T>(output.get<int64>());
	case 2: return static_cast<T>(output.get<float64>());
	default: return std::nullopt;
	}
}

template<typename T> requires std::is_pointer_v<T>
inline T SdlNativeData::get(const std::size_t elementIdx) const
{
	PH_ASSERT(m_elementGetter);

	using PtrRemovedT = std::remove_pointer_t<T>;

	SdlGetterVariant output = m_elementGetter(elementIdx);
	if constexpr(CSdlInstance<PtrRemovedT>)
	{
		return output.has<SdlConstInstance>()
			? output.get<SdlConstInstance>().get<PtrRemovedT>() : nullptr;
	}
	else
	{
		return output.has<AnyConstPtr>()
			? output.get<AnyConstPtr>().get<PtrRemovedT>() : nullptr;
	}
}

template<typename T> requires std::is_arithmetic_v<T> || std::is_enum_v<T>
inline bool SdlNativeData::set(const std::size_t elementIdx, const T value) const
{
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

template<typename T> requires std::is_null_pointer_v<T>
inline bool SdlNativeData::set(std::size_t elementIdx, T /* nullPtr */) const
{
	return m_elementSetter(elementIdx, std::monostate{});
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

inline bool SdlNativeData::isIntegerElement() const
{
	switch(elementType)
	{
	case ESdlDataType::Int8:
	case ESdlDataType::UInt8:
	case ESdlDataType::Int16:
	case ESdlDataType::UInt16:
	case ESdlDataType::Int32:
	case ESdlDataType::UInt32:
	case ESdlDataType::Int64:
	case ESdlDataType::UInt64:
	case ESdlDataType::Bool:
	case ESdlDataType::Enum:
		return true;

	default:
		return false;
	}
}

inline bool SdlNativeData::isFloatingPointElement() const
{
	switch(elementType)
	{
	case ESdlDataType::Float32:
	case ESdlDataType::Float64:
		return true;

	default:
		return false;
	}
}

inline SdlNativeData::operator bool () const
{
	return numElements > 0;
}

template<typename ElementType>
inline auto SdlNativeData::permissiveElementGetter(ElementType* const elementPtr)
-> SdlGetterVariant
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
inline auto SdlNativeData::permissiveElementSetter(SdlSetterVariant input, ElementType* const out_elementPtr)
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
			*out_elementPtr = static_cast<ElementType>(input.get<int64>());
			return true;

		case 2: 
			*out_elementPtr = static_cast<ElementType>(input.get<float64>());
			return true;

		default:
			return false;
		}
	}
	else if constexpr(CSdlInstance<ElementType>)
	{
		ElementType* const inputPtr = input.has<SdlNonConstInstance>()
			? input.get<SdlNonConstInstance>().get<ElementType>() : nullptr;
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
		ElementType* const inputPtr = input.has<AnyNonConstPtr>()
			? input.get<AnyNonConstPtr>().get<ElementType>() : nullptr;
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
