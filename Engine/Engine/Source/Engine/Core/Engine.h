#pragma once

#include "Engine/World/VisualWorld.h"
#include "Engine/SDL/SdlSceneFileReader.h"
#include "Engine/SDL/SceneDescription.h"
#include "Engine/Math/TVector2.h"
#include "Engine/Frame/frame_fwd.h"
#include "Engine/Frame/FrameProcessor.h"
#include "Engine/Core/Scheduler/Region.h"
#include "Engine/Core/Renderer/RenderObservableInfo.h"
#include "Engine/DataIO/FileSystem/Path.h"
#include "Engine/EngineEnv/CoreCookedUnit.h"

#include <Common/primitive_type.h>

#include <string>
#include <vector>

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

	/*! @brief Retrieve a completed frame.
	*/
	void retrieveFrame(
		uint32       layerIndex,
		HdrRgbFrame& out_frame,
		bool         applyPostProcessing = true);

	math::TVector2<int64> getFilmDimensionPx() const;

	/*! @brief Control the number of threads used for all engine operations.
	@param numThreads Number of threads. If set to 0, a default value will automatically be chosen.
	*/
	void setNumThreads(uint32 numThreads);

	/*! @brief Retrieve an intermediate frame.
	The contract matches @ref retrieveFrame() except that correctness is not guaranteed.
	*/
	void asyncPeekFrame(
		uint32        layerIndex,
		const Region& region,
		HdrRgbFrame&  out_frame, 
		bool          applyPostProcessing = true) const;

	void asyncQueryStatistics(float32* out_percentageProgress,
	                          float32* out_samplesPerSecond) const;

	void setWorkingDirectory(const Path& directory);

	/*! @brief Get descriptions for all currently observable render data.
	*/
	RenderObservableInfo getObservableInfo() const;

	Renderer* getRenderer() const;

	/*! @brief Get settings for each frame layer.
	*/
	const std::vector<FilmSetting>& getFilmSettings() const;

private:
	/*! @brief Apply the configured post-render pipeline to renderer output.
	*/
	void postProcessRendererFrame(
		uint32 layerIndex,
		HdrRgbFrame& frame,
		const math::TAABB2D<uint32>& region,
		bool applyPostProcessing) const;

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

inline const std::vector<FilmSetting>& Engine::getFilmSettings() const
{
	return m_cooked.getFilmSettings();
}

}// end namespace ph
