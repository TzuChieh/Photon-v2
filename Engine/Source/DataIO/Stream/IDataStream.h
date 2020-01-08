#pragma once

#include "Utility/INoncopyable.h"



namespace ph
{

class IDataStream : public INoncopyable
{
public:
	virtual ~IDataStream() = default;
};

}// end namespace ph
