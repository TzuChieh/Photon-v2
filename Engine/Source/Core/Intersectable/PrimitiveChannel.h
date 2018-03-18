#pragma once

#include "Core/Intersectable/UvwMapper/UvwMapper.h"

#include <memory>

namespace ph
{

class PrimitiveChannel final
{
public:
	PrimitiveChannel();
	PrimitiveChannel(const std::shared_ptr<UvwMapper>& mapper);

	inline const UvwMapper* getMapper() const
	{
		return m_mapper.get();
	}

private:
	std::shared_ptr<UvwMapper> m_mapper;
};

}// end namespace ph