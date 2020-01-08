#pragma once

#include "Utility/INoncopyable.h"

namespace ph
{

class IDataStream : public INoncopyable
{
public:
	virtual ~IDataStream() = default;

	virtual operator bool () const = 0;
};

}// end namespace ph
