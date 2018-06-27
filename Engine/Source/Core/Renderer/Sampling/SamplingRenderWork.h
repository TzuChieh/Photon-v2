#pragma once

#include "Core/Renderer/RenderWork.h"
#include "Core/Renderer/Sampling/SamplingStatistics.h"
#include "Core/Filmic/filmic_fwd.h"

#include <atomic>

namespace ph
{

class SamplingRenderer;
class Scene;
class Camera;
class Estimator;
class SampleGenerator;

class SamplingRenderWork : public RenderWork
{
public:
	SamplingRenderWork(
		SamplingRenderer*     renderer,
		const Scene*          scene,
		const Camera*         camera,
		const Estimator*      estimator,
		SampleGenerator*      sampleGenerator,
		SpectralSamplingFilm* film);

	SamplingRenderWork();
	SamplingRenderWork(const SamplingRenderWork& other);
	~SamplingRenderWork() override;

	void doWork() override;

	SamplingStatistics asyncGetStatistics();

	// HACK
	SpectralSamplingFilm* m_film;

private:
	SamplingRenderer*     m_renderer;
	const Scene*          m_scene;
	const Camera*         m_camera;
	const Estimator*      m_estimator;
	SampleGenerator*      m_sampleGenerator;

	std::atomic_uint32_t m_numSamplesTaken;
	std::atomic_uint32_t m_numMsElapsed;
};

// In-header Implementations:

inline SamplingRenderWork::SamplingRenderWork(

	SamplingRenderer* const     renderer,
	const Scene* const          scene,
	const Camera* const         camera,
	const Estimator* const      estimator,
	SampleGenerator* const      sampleGenerator,
	SpectralSamplingFilm* const film) :

	RenderWork(),

	m_renderer(renderer),
	m_scene(scene),
	m_camera(camera),
	m_estimator(estimator),
	m_sampleGenerator(sampleGenerator),
	m_film(film),

	m_numSamplesTaken(0),
	m_numMsElapsed(0)
{}

inline SamplingRenderWork::SamplingRenderWork() :
	SamplingRenderWork(nullptr, nullptr, nullptr, nullptr, nullptr, nullptr)
{}

inline SamplingRenderWork::~SamplingRenderWork() = default;

inline SamplingStatistics SamplingRenderWork::asyncGetStatistics()
{
	SamplingStatistics statistics;
	statistics.numSamplesTaken = m_numSamplesTaken;
	statistics.numMsElapsed    = m_numMsElapsed;
	
	return statistics;
}

}// end namespace ph