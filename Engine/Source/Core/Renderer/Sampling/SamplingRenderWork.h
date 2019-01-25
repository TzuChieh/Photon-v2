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
	SamplingRenderWork();

	SamplingRenderWork(
		const Estimator*  estimator,
		const Integrand&  integrand,
		SamplingRenderer* renderer);
	
	SamplingRenderWork(SamplingRenderWork&& other);

	SamplingStatistics asyncGetStatistics();

	void setFilms(SamplingFilmSet&& films);
	void setSampleGenerator(std::unique_ptr<SampleGenerator> sampleGenerator);
	void setRequestedAttributes(const AttributeTags& attributes);
	void setDomainPx(const TAABB2D<int64>& domainPx);

	SamplingRenderWork& operator = (SamplingRenderWork&& rhs);

private:
	void doWork() override;

	Integrand         m_integrand;
	const Estimator*  m_estimator;
	SamplingRenderer* m_renderer;

	SamplingFilmSet                  m_films;
	std::unique_ptr<SampleGenerator> m_sampleGenerator;
	AttributeTags                    m_requestedAttributes;

	std::atomic_uint32_t m_numSamplesTaken;
};

// In-header Implementations:

inline SamplingRenderWork::SamplingRenderWork(
	const Estimator*  estimator,
	const Integrand&  integrand,
	SamplingRenderer* renderer) :

	RenderWork(),

	m_estimator(estimator),
	m_integrand(integrand),
	m_renderer(renderer),

	m_sampleGenerator(nullptr),
	m_films(),
	m_requestedAttributes(),

	m_numSamplesTaken(0)
{}

inline SamplingRenderWork::SamplingRenderWork() :
	SamplingRenderWork(nullptr, Integrand(), nullptr)
{}

inline SamplingStatistics SamplingRenderWork::asyncGetStatistics()
{
	SamplingStatistics statistics;
	statistics.numSamplesTaken = m_numSamplesTaken;
	
	return statistics;
}

inline void SamplingRenderWork::setFilms(SamplingFilmSet&& films)
{
	m_films = std::move(films);
}

inline void SamplingRenderWork::setSampleGenerator(std::unique_ptr<SampleGenerator> sampleGenerator)
{
	m_sampleGenerator = std::move(sampleGenerator);
}

inline void SamplingRenderWork::setRequestedAttributes(const AttributeTags& attributes)
{
	m_requestedAttributes = attributes;
}

inline SamplingRenderWork& SamplingRenderWork::operator = (SamplingRenderWork&& rhs)
{
	RenderWork::operator = (std::move(rhs));

	m_estimator           = rhs.m_estimator;
	m_integrand           = rhs.m_integrand;
	m_renderer            = rhs.m_renderer;
	m_films               = std::move(rhs.m_films);
	m_sampleGenerator     = std::move(rhs.m_sampleGenerator);
	m_requestedAttributes = std::move(rhs.m_requestedAttributes);
	m_numSamplesTaken     = rhs.m_numSamplesTaken.load();

	return *this;
}

}// end namespace ph