#pragma once

#include "RenderCore/GHI.h"

namespace ph::editor
{

class NullGHI : public GHI
{
public:
	inline NullGHI()
		: GHI(EGraphicsAPI::Unknown)
	{}

	inline ~NullGHI() override = default;

	inline void load() override
	{}

	inline void unload() override
	{}

	inline void setViewport(uint32 xPx, uint32 yPx, uint32 widthPx, uint32 heightPx) override
	{}

	inline void clearBuffer(EClearTarget targets) override
	{}

	inline void setClearColor(const math::Vector4F& color) override
	{}

	inline void swapBuffers() override
	{}

	inline std::shared_ptr<GHITexture2D> createTexture2D(
		const math::TVector2<uint32>& sizePx,
		EGHITextureFormat format,
		const GHISampleState& state) override
	{
		return nullptr;
	}

private:
	inline void beginRawCommand() override
	{}

	inline void endRawCommand() override
	{}
};

}// end namespace ph::editor
