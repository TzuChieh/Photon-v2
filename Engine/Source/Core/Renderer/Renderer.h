#pragma once

#include "Common/primitive_type.h"
#include "Core/Renderer/Renderer.h"
#include "Core/Renderer/RenderWorker.h"

#include <vector>
#include <mutex>
#include <memory>
#include <atomic>

namespace ph
{

class Film;
class Sample;
class VisualWorld;
class Camera;
class Description;

class Renderer final
{
public:
	Renderer();
	~Renderer();

	void render(const Description& description) const;
	float32 queryPercentageProgress() const;
	float32 querySampleFrequency() const;

	void setNumRenderThreads(const uint32 numThreads);

private:
	uint32 m_numThreads;

	//mutable std::vector<Film> m_subFilms;
	mutable std::mutex m_rendererMutex;

	mutable std::vector<RenderWorker> m_workers;

	//std::vector<std::unique_ptr<std::atomic<float32>>> m_workerProgresses;
	//std::vector<std::unique_ptr<std::atomic<float32>>> m_workerSampleFrequencies;
};

}// end namespace ph