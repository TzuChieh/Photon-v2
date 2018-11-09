#pragma once

#include "Core/Renderer/RenderWork.h"
#include "Core/Renderer/Sampling/SamplingStatistics.h"
#include "Core/Renderer/Sampling/SamplingFilmSet.h"
#include "Core/SampleGenerator/SampleGenerator.h"
#include "Core/Filmic/TSamplingFilm.h"
#include "Utility/INoncopyable.h"
#include "Core/Estimator/Integrand.h"
#include "Core/Filmic/SampleFilter.h"
#include "Core/Renderer/AttributeTags.h"

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
		SamplingFilmSet films,
		std::unique_ptr<SampleGenerator> sampleGenerator,
		const AttributeTags& requestedAttributes);

	SamplingRenderWork();
	SamplingRenderWork(SamplingRenderWork&& other);
	~SamplingRenderWork() override;

	SamplingStatistics asyncGetStatistics();
	void setDomainPx(const TAABB2D<int64>& domainPx);

	SamplingRenderWork& operator = (SamplingRenderWork&& rhs);

	// HACK
	SamplingFilmSet m_films;

private:
	void doWork() override;

	SamplingRenderer*     m_renderer;
	Integrand             m_integrand;
	const Estimator*      m_estimator;
	std::unique_ptr<SampleGenerator> m_sampleGenerator;
	AttributeTags m_requestedAttributes;

	std::atomic_uint32_t m_numSamplesTaken;
};

// In-header Implementations:

inline SamplingRenderWork::SamplingRenderWork(
	SamplingRenderer* renderer,
	const Estimator* estimator,
	const Integrand& integrand,
	SamplingFilmSet films,
	std::unique_ptr<SampleGenerator> sampleGenerator,
	const AttributeTags& requestedAttributes) :

	RenderWork(),

	m_renderer(renderer),
	m_integrand(integrand),
	m_estimator(estimator),
	m_sampleGenerator(std::move(sampleGenerator)),
	m_films(std::move(films)),
	m_requestedAttributes(requestedAttributes),

	m_numSamplesTaken(0)
{}

inline SamplingRenderWork::SamplingRenderWork() :
	SamplingRenderWork(nullptr, nullptr, Integrand(), SamplingFilmSet(), nullptr, AttributeTags())
{}

inline SamplingRenderWork::~SamplingRenderWork() = default;

inline SamplingStatistics SamplingRenderWork::asyncGetStatistics()
{
	SamplingStatistics statistics;
	statistics.numSamplesTaken = m_numSamplesTaken;
	
	return statistics;
}

inline SamplingRenderWork& SamplingRenderWork::operator = (SamplingRenderWork&& rhs)
{
	RenderWork::operator = (std::move(rhs));

	m_renderer            = std::move(rhs.m_renderer);
	m_integrand           = std::move(rhs.m_integrand);
	m_estimator           = std::move(rhs.m_estimator);
	m_sampleGenerator     = std::move(rhs.m_sampleGenerator);
	m_films               = std::move(rhs.m_films);
	m_requestedAttributes = std::move(rhs.m_requestedAttributes);

	m_numSamplesTaken = rhs.m_numSamplesTaken.load();

	return *this;
}

}// end namespace ph