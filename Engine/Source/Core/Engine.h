#pragma once

#include "World/VisualWorld.h"
#include "Core/Renderer/Renderer.h"
#include "FileIO/DescriptionParser.h"
#include "FileIO/Description.h"
#include "Math/TVector2.h"
#include "Core/Renderer/Renderer.h"
#include "Core/Renderer/ERegionStatus.h"
#include "Frame/frame_fwd.h"
#include "Core/Filmic/FilmSet.h"

#include <string>
#include <memory>

namespace ph
{

class Path;

class Engine final
{
public:
	Engine();

	void enterCommand(const std::string& commandFragment);
	void update();
	void render();
	void developFilm(HdrRgbFrame& out_frame, bool applyPostProcessing = true);
	TVector2<int64> getFilmDimensionPx() const;
	void setNumRenderThreads(uint32 numThreads);

	ERegionStatus asyncPollUpdatedRegion(Renderer::Region* out_region) const;
	void asyncDevelopFilmRegion(HdrRgbFrame& out_frame, const Renderer::Region& region, bool applyPostProcessing = true) const;
	void asyncQueryStatistics(float32* out_percentageProgress,
	                          float32* out_samplesPerSecond) const;

	void setWorkingDirectory(const Path& path);

private:
	DescriptionParser m_parser;
	Description m_description;
	std::unique_ptr<Renderer> m_renderer;

	FilmSet m_filmSet;
};

}// end namespace ph