#pragma once

#include "Engine/World/VisualWorld.h"
#include "Engine/SDL/SdlSceneFileReader.h"
#include "Engine/SDL/SceneDescription.h"
#include "Engine/Math/TVector2.h"
#include "Engine/Frame/frame_fwd.h"
#include "Engine/Frame/FrameProcessor.h"
#include "Engine/Core/Scheduler/Region.h"
#include "Engine/DataIO/FileSystem/Path.h"
#include "Engine/EngineEnv/CoreCookedUnit.h"

#include <Common/primitive_type.h>

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
		int32        layerIndex,
		HdrRgbFrame& out_frame,
		bool         applyPostProcessing = true);

	math::TVector2<int64> getFilmDimensionPx() const;

	/*! @brief Control the number of threads used for all engine operations.
	@param numThreads Number of threads. If set to 0, a default value will automatically be chosen.
	*/
	void setNumThreads(uint32 numThreads);

	void asyncPeekFrame(
		int32         layerIndex,
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
