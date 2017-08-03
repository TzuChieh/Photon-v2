#pragma once

#include "World/VisualWorld.h"
#include "Core/Renderer/Renderer.h"
#include "FileIO/DescriptionParser.h"
#include "FileIO/Description.h"
#include "Math/TVector2.h"
#include "Core/Renderer/Renderer.h"
#include "Core/Renderer/ERegionStatus.h"

#include <string>
#include <memory>

namespace ph
{

class Frame;

class Engine final
{
public:
	Engine();

	void enterCommand(const std::string& commandFragment);
	void render();
	void developFilm(Frame& out_frame);
	TVector2<int64> getFilmDimensionPx() const;
	void setNumRenderThreads(uint32 numThreads);

	float32 asyncQueryPercentageProgress() const;
	float32 asyncQuerySampleFrequency() const;
	ERegionStatus asyncPollUpdatedRegion(Renderer::Region* out_region) const;
	void asyncDevelopFilmRegion(Frame& out_frame, const Renderer::Region& region) const;

private:
	DescriptionParser m_parser;
	Description m_description;
	std::unique_ptr<Renderer> m_renderer;
};

}// end namespace ph