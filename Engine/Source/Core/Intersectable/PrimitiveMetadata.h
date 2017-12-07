#pragma once

#include "Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Math/Transform/StaticTransform.h"
#include "Core/Texture/UvwMapper/UvwMapper.h"
#include "Core/Texture/TextureChannel.h"

#include <memory>
#include <vector>

namespace ph
{

class PrimitiveMetadata final
{
public:
	SurfaceBehavior surfaceBehavior;

	PrimitiveMetadata();

	int addTextureChannel(const TextureChannel& channel);

	inline const TextureChannel& getTextureChannel(const int channelId) const
	{
		return m_textureChannels[channelId];
	}

	inline const TextureChannel& getDefaultTextureChannel() const
	{
		return getTextureChannel(0);
	}

private:
	std::vector<TextureChannel> m_textureChannels;
};

}// end namespace ph