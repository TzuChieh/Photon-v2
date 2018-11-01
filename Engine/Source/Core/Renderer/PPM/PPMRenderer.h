#pragma once

#include "Core/Renderer/Renderer.h"
#include "Core/Filmic/HdrRgbFilm.h"
#include "Core/Filmic/SampleFilter.h"
#include "Core/Renderer/Sampling/SamplingRenderWork.h"
#include "Core/Renderer/Sampling/SamplingFilmSet.h"

#include <vector>
#include <memory>
#include <atomic>

namespace ph
{

class PPMRenderer final : public Renderer, public TCommandInterface<SamplingRenderer>
{
public:
	AttributeTags supportedAttributes() const override;
	void init(const SdlResourcePack& data) override;
	bool asyncSupplyWork(RenderWorker& worker) override;
	void asyncSubmitWork(RenderWorker& worker) override;
	ERegionStatus asyncPollUpdatedRegion(Region* out_region) override;
	RenderStates asyncQueryRenderStates() override;
	void asyncDevelopFilmRegion(HdrRgbFrame& out_frame, const Region& region, EAttribute attribute) override;
	void develop(HdrRgbFrame& out_frame, EAttribute attribute) override;

private:
	HdrRgbFilm m_film;
	const Scene*          m_scene;
	Camera*               m_camera;
	SampleGenerator*      m_sg;
	SampleFilter          m_filter;

// command interface
public:
	explicit PPMRenderer(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
};

}// end namespace ph