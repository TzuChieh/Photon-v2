#pragma once

#include "SDL/Introspect/SdlNativeData.h"
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

//template<typename T>
//inline T* SdlNativeData::directAccess() const
//{
//	if(m_directType == typeid(T))
//	{
//		return static_cast<T*>(m_directPtr);
//	}
//	else
//	{
//		return nullptr;
//	}
//}

inline void* SdlNativeData::operator [] (const std::size_t elementIdx) const
{
	PH_ASSERT_LT(elementIdx, numElements);
	return m_elementGetter(elementIdx);
}

inline SdlNativeData::operator bool() const
{
	return numElements > 0;
}

}// end namespace ph
