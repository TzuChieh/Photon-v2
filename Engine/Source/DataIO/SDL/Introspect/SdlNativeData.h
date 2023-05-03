#pragma once

#include "DataIO/SDL/ESdlDataFormat.h"
#include "DataIO/SDL/ESdlDataType.h"
#include "Utility/TFunction.h"

#include <cstddef>

namespace ph
{

class SdlNativeData final
{
public:
	ESdlDataFormat format = ESdlDataFormat::None;
	ESdlDataType dataType = ESdlDataType::None;
	std::size_t numElements = 0;

	SdlNativeData();

	template<typename ElementType>
	explicit SdlNativeData(ElementType* elementPtr);

	template<typename ElementGetterFunc>
	SdlNativeData(ElementGetterFunc func, std::size_t numElements);

	void* operator [] (std::size_t elementIdx) const;

	operator bool() const;

private:
	TFunction<void* (std::size_t elementIdx)> m_elementGetter;
};

}// end namespace ph

#include "DataIO/SDL/Introspect/SdlNativeData.ipp"
