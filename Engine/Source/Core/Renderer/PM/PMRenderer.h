#pragma once

#include "Core/Renderer/Renderer.h"
#include "Core/Filmic/HdrRgbFilm.h"
#include "Core/Filmic/SampleFilter.h"
#include "Core/Renderer/PM/EPMMode.h"

#include <vector>
#include <memory>
#include <atomic>
#include <cstddef>
#include <mutex>

namespace ph
{

class PMRenderer : public Renderer, public TCommandInterface<PMRenderer>
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

	EPMMode m_mode;
	std::size_t m_numPhotons;
	real m_kernelRadius;
	std::size_t m_perPixelSamples;

	std::mutex m_filmMutex;

	void renderWithVanillaPM();

// command interface
public:
	explicit PMRenderer(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
};

}// end namespace ph