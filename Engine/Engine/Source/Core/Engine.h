#pragma once

#include "World/VisualWorld.h"
#include "SDL/SdlSceneFileReader.h"
#include "SDL/SceneDescription.h"
#include "Math/TVector2.h"
#include "Frame/frame_fwd.h"
#include "Frame/FrameProcessor.h"
#include "Core/Scheduler/Region.h"
#include "DataIO/FileSystem/Path.h"
#include "EngineEnv/CoreCookedUnit.h"

#include <string>

namespace ph
{

/*! @brief The render engine.
*/
class Engine final
{
public:
	Engine();

	void enterCommand(const std::string& commandFragment);
	bool loadCommands(const Path& sceneFile);
	void update();
	void render();

	void retrieveFrame(
		std::size_t  layerIndex,
		HdrRgbFrame& out_frame,
		bool         applyPostProcessing = true);

	math::TVector2<int64> getFilmDimensionPx() const;

	/*! @brief Control the number of threads used for all engine operations.
	@param numThreads Number of threads. If set to 0, a default value will automatically be chosen.
	*/
	void setNumThreads(uint32 numThreads);

	void asyncPeekFrame(
		std::size_t   layerIndex,
		const Region& region,
		HdrRgbFrame&  out_frame, 
		bool          applyPostProcessing = true) const;

	void asyncQueryStatistics(float32* out_percentageProgress,
	                          float32* out_samplesPerSecond) const;

	void setWorkingDirectory(const Path& directory);

	Renderer* getRenderer() const;

private:
	SdlSceneFileReader m_sceneParser;
	SceneDescription m_rawScene;
	VisualWorld m_visualWorld;
	CoreCookedUnit m_cooked;
	uint32 m_numThreads;

	//FrameProcessor m_frameProcessor;
	// TODO: associate each attribute with a pipeline
	//FrameProcessor::PipelineId m_id;
};

// In-header Implementations:

inline Renderer* Engine::getRenderer() const
{
	return m_cooked.getRenderer();
}

}// end namespace ph
