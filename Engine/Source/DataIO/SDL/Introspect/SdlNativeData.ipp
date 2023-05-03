#pragma once

#include "DataIO/SDL/Introspect/SdlNativeData.h"
#include "Common/assertion.h"

#include <utility>

namespace ph
{

inline SdlNativeData::SdlNativeData()
	: SdlNativeData(
		[](std::size_t /* elementIdx */) -> void*
		{
			return nullptr;
		},
		0)
{}

template<typename ElementType>
inline SdlNativeData::SdlNativeData(ElementType* const elementPtr)
	: SdlNativeData(
		[elementPtr](std::size_t /* elementIdx */) -> void*
		{
			return elementPtr;
		},
		1)
{}

template<typename ElementGetterFunc>
inline SdlNativeData::SdlNativeData(ElementGetterFunc func, const std::size_t numElements)
	: m_elementGetter(std::move(func))
	, numElements(numElements)
{}

inline void* SdlNativeData::operator [] (const std::size_t elementIdx) const
{
	PH_ASSERT_LT(elementIdx, numElements);
	return m_elementGetter(elementIdx);
}

inline SdlNativeData::operator bool() const
{
	return m_elementGetter.isValid();
}

}// end namespace ph
