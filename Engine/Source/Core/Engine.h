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

template<typename ComponentType>
class TFrame;

class Path;

class Engine final
{
public:
	Engine();

	void enterCommand(const std::string& commandFragment);
	void update();
	void render();
	void developFilm(TFrame<real>& out_frame);
	TVector2<int64> getFilmDimensionPx() const;
	void setNumRenderThreads(uint32 numThreads);

	ERegionStatus asyncPollUpdatedRegion(Renderer::Region* out_region) const;
	void asyncDevelopFilmRegion(TFrame<real>& out_frame, const Renderer::Region& region) const;
	void asyncQueryStatistics(float32* out_percentageProgress,
	                          float32* out_samplesPerSecond) const;

	void setWorkingDirectory(const Path& path);

private:
	DescriptionParser m_parser;
	Description m_description;
	std::unique_ptr<Renderer> m_renderer;
};

}// end namespace ph