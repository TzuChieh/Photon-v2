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
#include "Core/Filmic/HdrRgbFilm.h"
#include "Core/Estimator/estimator_fwd.h"

#include <atomic>
#include <functional>

namespace ph
{

class SamplingRenderer;
class Scene;
class Camera;
class SampleFilter;

class SamplingRenderWork : public RenderWork, public INoncopyable
{
public:
	SamplingRenderWork();

	SamplingRenderWork(
		const IEnergyEstimator* estimator,
		const Integrand&  integrand,
		SamplingRenderer* renderer);
	
	SamplingRenderWork(SamplingRenderWork&& other);

	SamplingStatistics asyncGetStatistics();

	void setFilm(HdrRgbFilm* film);
	void setSampleGenerator(std::unique_ptr<SampleGenerator> sampleGenerator);
	void setRequestedAttributes(const AttributeTags& attributes);
	void setDomainPx(const TAABB2D<int64>& domainPx);

	void onWorkStart(std::function<void()> func);
	void onWorkReport(std::function<void()> func);
	void onWorkFinish(std::function<void()> func);

	SamplingRenderWork& operator = (SamplingRenderWork&& rhs);

private:
	void doWork() override;

	Integrand         m_integrand;
	const IEnergyEstimator* m_estimator;
	SamplingRenderer* m_renderer;

	HdrRgbFilm* m_film;
	std::unique_ptr<SampleGenerator> m_sampleGenerator;
	AttributeTags                    m_requestedAttributes;

	std::atomic_uint32_t m_numSamplesTaken;
	std::function<void()> m_onWorkStart;
	std::function<void()> m_onWorkReport;
	std::function<void()> m_onWorkFinish;
};

// In-header Implementations:

inline SamplingRenderWork::SamplingRenderWork(
	const IEnergyEstimator*  estimator,
	const Integrand&  integrand,
	SamplingRenderer* renderer) :

	RenderWork(),

	m_estimator(estimator),
	m_integrand(integrand),
	m_renderer(renderer),

	m_sampleGenerator(nullptr),
	m_film(nullptr),
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

inline void SamplingRenderWork::onWorkStart(std::function<void()> func)
{
	m_onWorkStart = std::move(func);
}

inline void SamplingRenderWork::onWorkReport(std::function<void()> func)
{
	m_onWorkReport = std::move(func);
}

inline void SamplingRenderWork::onWorkFinish(std::function<void()> func)
{
	m_onWorkFinish = std::move(func);
}

inline void SamplingRenderWork::setFilm(HdrRgbFilm* film)
{
	m_film = std::move(film);
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
	m_film               = rhs.m_film;
	m_sampleGenerator     = std::move(rhs.m_sampleGenerator);
	m_requestedAttributes = std::move(rhs.m_requestedAttributes);
	m_numSamplesTaken     = rhs.m_numSamplesTaken.load();

	m_onWorkStart = std::move(rhs.m_onWorkStart);
	m_onWorkReport = std::move(rhs.m_onWorkReport);
	m_onWorkFinish = std::move(rhs.m_onWorkFinish);

	return *this;
}

}// end namespace ph