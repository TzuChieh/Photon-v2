#pragma once

#include "DataIO/SDL/ESdlDataFormat.h"
#include "DataIO/SDL/ESdlDataType.h"

namespace ph
{

class SdlNativeData final
{
public:
	ESdlDataFormat format = ESdlDataFormat::None;
	ESdlDataType dataType = ESdlDataType::None;
	void* dataPtr = nullptr;

	operator bool() const;
};

inline SdlNativeData::operator bool() const
{
	return dataPtr != nullptr;
}

}// end namespace ph
