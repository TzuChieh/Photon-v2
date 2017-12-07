#include "Core/Texture/TextureChannel.h"
#include "Core/Texture/UvwMapper/StAsUvMapper.h"

namespace ph
{

TextureChannel::TextureChannel() : 
	m_mapper(std::make_shared<StAsUvMapper>())
{

}

TextureChannel::TextureChannel(std::shared_ptr<UvwMapper> mapper) :
	m_mapper(std::move(mapper))
{

}

}// end namespace ph