#pragma once

#include "World/VisualWorld.h"
#include "DataIO/SDL/SdlParser.h"
#include "DataIO/SDL/SceneDescription.h"
#include "Math/TVector2.h"
#include "Core/Renderer/ERegionStatus.h"
#include "Frame/frame_fwd.h"
#include "Frame/FrameProcessor.h"
#include "Core/Renderer/EAttribute.h"
#include "Core/Renderer/Region/Region.h"
#include "DataIO/FileSystem/Path.h"
#include "Actor/CoreCookedUnit.h"

#include <string>

namespace ph
{

class Engine final
{
public:
	Engine();

	void enterCommand(const std::string& commandFragment);
	bool loadCommands(const Path& filePath);
	void update();
	void render();

	void retrieveFrame(
		std::size_t  layerIndex,
		HdrRgbFrame& out_frame,
		bool         applyPostProcessing = true);

	math::TVector2<int64> getFilmDimensionPx() const;
	void setNumRenderThreads(uint32 numThreads);

	ERegionStatus asyncPollUpdatedRegion(Region* out_region) const;

	void asyncPeekFrame(
		std::size_t   layerIndex,
		const Region& region,
		HdrRgbFrame&  out_frame, 
		bool          applyPostProcessing = true) const;

	void asyncQueryStatistics(float32* out_percentageProgress,
	                          float32* out_samplesPerSecond) const;

	void setWorkingDirectory(const Path& path);

	Renderer* getRenderer() const;

private:
	SdlParser        m_parser;
	SceneDescription m_scene;
	VisualWorld      m_visualWorld;
	CoreCookedUnit   m_core;
	uint32           m_numRenderThreads;

	//FrameProcessor m_frameProcessor;
	// TODO: associate each attribute with a pipeline
	//FrameProcessor::PipelineId m_id;
};

// In-header Implementations:

inline Renderer* Engine::getRenderer() const
{
	return m_coreData.getRenderer();
}

}// end namespace ph
