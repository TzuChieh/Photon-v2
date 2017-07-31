#pragma once

#include "Common/primitive_type.h"
#include "Core/Renderer/RenderWorker.h"
#include "Core/SampleGenerator/SampleGenerator.h"
#include "Core/Camera/Camera.h"
#include "Core/Bound/TAABB2D.h"

#include <vector>
#include <mutex>
#include <memory>
#include <atomic>
#include <deque>

namespace ph
{

class Description;

class Renderer final
{
public:
	typedef TAABB2D<int64> Region;

public:
	Renderer();
	~Renderer();

	void render(const Description& description);
	float32 asyncQueryPercentageProgress() const;
	float32 asyncQuerySampleFrequency() const;

	void setNumRenderThreads(const uint32 numThreads);
	void asyncAddUpdatedRegion(const Region& region);
	bool asyncPollUpdatedRegion(Region* out_region);

private:
	uint32 m_numThreads;

	//mutable std::vector<Film> m_subFilms;
	std::mutex m_rendererMutex;

	
	std::vector<RenderWorker>                     m_workers;
	std::vector<std::unique_ptr<SampleGenerator>> m_workerSgs;
	std::vector<std::unique_ptr<Film>>            m_workerFilms;
	std::deque<Region>                            m_updatedRegions;
	

	//std::vector<std::unique_ptr<std::atomic<float32>>> m_workerProgresses;
	//std::vector<std::unique_ptr<std::atomic<float32>>> m_workerSampleFrequencies;

	void clearWorkerData();
	void genFullRegionRenderWorkers(const Description& description, uint32 numWorkers);
};

}// end namespace ph