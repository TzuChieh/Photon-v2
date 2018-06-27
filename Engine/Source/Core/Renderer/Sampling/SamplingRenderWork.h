#pragma once

#include "Core/Renderer/RenderWork.h"
#include "Core/Renderer/Sampling/SamplingStatistics.h"
#include "Core/Filmic/filmic_fwd.h"
#include "Core/SampleGenerator/SampleGenerator.h"
#include "Core/Filmic/TSamplingFilm.h"
#include "Utility/INoncopyable.h"
#include "Core/Estimator/Integrand.h"
#include "Core/Bound/TAABB2D.h"
#include "Core/Filmic/SampleFilter.h"

#include <atomic>

namespace ph
{

class SamplingRenderer;
class Scene;
class Camera;
class Estimator;
class SampleFilter;

class SamplingRenderWork : public RenderWork, public INoncopyable
{
public:
	SamplingRenderWork(
		SamplingRenderer* renderer,
		const Estimator* estimator,
		const Integrand& integrand,
		std::unique_ptr<SpectralSamplingFilm> film,
		std::unique_ptr<SampleGenerator> sampleGenerator);

	SamplingRenderWork();
	SamplingRenderWork(SamplingRenderWork&& other);
	~SamplingRenderWork() override;

	void doWork() override;

	void setDomainPx(const TAABB2D<int64>& domainPx);

	SamplingStatistics asyncGetStatistics();

	// HACK
	std::unique_ptr<SpectralSamplingFilm> m_film;

private:
	SamplingRenderer*     m_renderer;
	Integrand             m_integrand;
	const Estimator*      m_estimator;
	std::unique_ptr<SampleGenerator> m_sampleGenerator;
	TAABB2D<int64> m_domainPx;

	std::atomic_uint32_t m_numSamplesTaken;
	std::atomic_uint32_t m_numMsElapsed;
};

// In-header Implementations:

inline SamplingRenderWork::SamplingRenderWork(
	SamplingRenderer* renderer,
	const Estimator* estimator,
	const Integrand& integrand,
	std::unique_ptr<SpectralSamplingFilm> film,
	std::unique_ptr<SampleGenerator> sampleGenerator) :

	RenderWork(),

	m_renderer(renderer),
	m_integrand(integrand),
	m_estimator(estimator),
	m_sampleGenerator(std::move(sampleGenerator)),
	m_film(std::move(film)),

	m_numSamplesTaken(0),
	m_numMsElapsed(0)
{}

inline SamplingRenderWork::SamplingRenderWork() :
	SamplingRenderWork(nullptr, nullptr, Integrand(), nullptr, nullptr)
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