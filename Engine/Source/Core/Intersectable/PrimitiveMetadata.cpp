#include "Core/Intersectable/PrimitiveMetadata.h"
#include "Core/Texture/TextureChannel.h"

#include <iostream>

namespace ph
{

PrimitiveMetadata::PrimitiveMetadata() :
	surfaceBehavior(),
	m_textureChannels()
{
	addTextureChannel(TextureChannel());
}

int PrimitiveMetadata::addTextureChannel(const TextureChannel& channel)
{
	m_textureChannels.push_back(channel);
	return static_cast<int>(m_textureChannels.size() - 1);
}

}// end namespace ph