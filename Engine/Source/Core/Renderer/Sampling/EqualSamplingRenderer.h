#pragma once

#include "Core/Renderer/Sampling/SamplingRenderer.h"
#include "Core/Filmic/HdrRgbFilm.h"
#include "Core/Renderer/Sampling/ReceiverSamplingWork.h"
#include "Core/Renderer/Sampling/TReceiverMeasurementEstimator.h"
#include "Core/Renderer/Region/WorkScheduler.h"
#include "Core/Renderer/Sampling/MetaRecordingProcessor.h"
#include "Core/Quantity/Spectrum.h"
#include "Math/TVector2.h"

#include <vector>
#include <memory>
#include <atomic>
#include <functional>

namespace ph
{

class Scene;
class Receiver;
class SampleGenerator;

class EqualSamplingRenderer : public SamplingRenderer
{
public:
	void doUpdate(const CoreDataGroup& data) override;
	void doRender() override;
	void retrieveFrame(std::size_t layerIndex, HdrRgbFrame& out_frame) override;

	ERegionStatus asyncPollUpdatedRegion(Region* out_region) override;
	RenderState asyncQueryRenderState() override;
	RenderProgress asyncQueryRenderProgress() override;
	void asyncPeekFrame(
		std::size_t   layerIndex,
		const Region& region,
		HdrRgbFrame&  out_frame) override;

	ObservableRenderData getObservableData() const override;

private:
	using FilmEstimator = TReceiverMeasurementEstimator<HdrRgbFilm, Spectrum>;

	enum class EScheduler
	{
		BULK,
		STRIPE,
		GRID,
		TILE,
		SPIRAL,
		SPIRAL_GRID
	};

	const Scene*                   m_scene;
	const Receiver*                m_receiver;
	SampleGenerator*               m_sampleGenerator;
	HdrRgbFilm                     m_mainFilm;

	std::unique_ptr<WorkScheduler> m_scheduler;
	EScheduler                     m_schedulerType;
	math::Vector2S                 m_blockSize;
	
	std::vector<ReceiverSamplingWork>       m_renderWorks;
	std::vector<FilmEstimator>              m_filmEstimators;
	std::vector<MetaRecordingProcessor>     m_metaRecorders;

	struct UpdatedRegion
	{
		Region region;
		bool   isFinished;
	};
	std::deque<UpdatedRegion> m_updatedRegions;
	
	std::mutex           m_rendererMutex;
	std::atomic_uint64_t m_totalPaths;
	std::atomic_uint32_t m_suppliedFractionBits;
	std::atomic_uint32_t m_submittedFractionBits;

	void addUpdatedRegion(const Region& region, bool isUpdating);
	void initScheduler(std::size_t numSamplesPerPixel);
};

}// end namespace ph
