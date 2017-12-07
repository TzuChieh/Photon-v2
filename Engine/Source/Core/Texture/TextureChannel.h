#pragma once

#include "Core/Texture/UvwMapper/UvwMapper.h"

#include <memory>

namespace ph
{

class TextureChannel final
{
public:
	TextureChannel();
	TextureChannel(std::shared_ptr<UvwMapper> mapper);

	inline const UvwMapper* getMapper() const
	{
		return m_mapper.get();
	}

private:
	std::shared_ptr<UvwMapper> m_mapper;
};

}// end namespace ph