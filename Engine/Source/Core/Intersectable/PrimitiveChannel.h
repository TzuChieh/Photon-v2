#pragma once

#include "Core/Intersectable/UvwMapper/UvwMapper.h"

#include <memory>

namespace ph
{

/*
	Represents a channel for some primitives. Each channel contains data 
	that can be utilized by the corresponding primitives, such as 
	UVW mappers. Channels can be easily switched via channel index, 
	resulting in different data being used for any channel-aware methods.
*/
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