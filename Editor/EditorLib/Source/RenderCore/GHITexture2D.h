#pragma once

#include "RenderCore/GHITexture.h"

#include <Math/TVector2.h>
#include <Common/primitive_type.h>

#include <cstddef>
#include <variant>

namespace ph::editor
{

class GHITexture2D : public GHITexture
{
public:
	GHITexture2D();
	~GHITexture2D() override;

	void upload(
		const std::byte* pixelData, 
		std::size_t numBytes, 
		EGHIInfoPixelComponent componentType) override = 0;

	void bind(uint32 slotIndex) override = 0;
};

}// end namespace ph::editor
