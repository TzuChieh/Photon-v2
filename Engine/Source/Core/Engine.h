#pragma once

#include "World/VisualWorld.h"
#include "Core/Renderer/Renderer.h"
#include "FileIO/SDL/SdlParser.h"
#include "FileIO/SDL/SdlResourcePack.h"
#include "Math/TVector2.h"
#include "Core/Renderer/Renderer.h"
#include "Core/Renderer/ERegionStatus.h"
#include "Frame/frame_fwd.h"
#include "Frame/FrameProcessor.h"
#include "Core/Renderer/EAttribute.h"
#include "Core/Renderer/Region/Region.h"
#include "FileIO/FileSystem/Path.h"

#include <string>
#include <memory>

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
	SdlParser       m_parser;
	SdlResourcePack m_data;
	std::shared_ptr<Renderer> m_renderer;
	uint32 m_numRenderThreads;

	//FrameProcessor m_frameProcessor;
	// TODO: associate each attribute with a pipeline
	//FrameProcessor::PipelineId m_id;
};

// In-header Implementations:

inline Renderer* Engine::getRenderer() const
{
	PH_ASSERT(m_renderer);

	return m_renderer.get();
}

}// end namespace ph
