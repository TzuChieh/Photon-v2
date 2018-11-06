#pragma once

#include "Core/Renderer/Renderer.h"
#include "Core/Filmic/HdrRgbFilm.h"
#include "Core/Filmic/SampleFilter.h"

#include <vector>
#include <memory>
#include <atomic>

namespace ph
{

class PPMRenderer final : public Renderer, public TCommandInterface<PPMRenderer>
{
public:
	void doUpdate(const SdlResourcePack& data) override;
	void doRender() override;
	void develop(HdrRgbFrame& out_frame, EAttribute attribute) override;

	ERegionStatus asyncPollUpdatedRegion(Region* out_region) override;
	RenderState asyncQueryRenderState() override;
	RenderProgress asyncQueryRenderProgress() override;
	void asyncDevelopRegion(HdrRgbFrame& out_frame, const Region& region, EAttribute attribute) override;

	AttributeTags supportedAttributes() const override;
	std::string renderStateName(RenderState::EType type, std::size_t index) const override;

private:
	std::unique_ptr<HdrRgbFilm> m_film;

	const Scene*          m_scene;
	const Camera*         m_camera;
	SampleGenerator*      m_sg;
	SampleFilter          m_filter;

// command interface
public:
	explicit PPMRenderer(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
};

}// end namespace ph