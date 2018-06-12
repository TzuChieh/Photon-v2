#include "Core/Integrator/AbstractPathIntegrator.h"
#include "Common/assertion.h"
#include "Core/Filmic/HdrRgbFilm.h"
#include "Core/Filmic/SampleFilterFactory.h"
#include "Core/SampleGenerator/SampleGenerator.h"
#include "Core/Camera/Camera.h"
#include "World/Scene.h"
#include "Core/SurfaceHit.h"
#include "FileIO/SDL/InputPacket.h"

#include <iostream>

namespace ph
{

AbstractPathIntegrator::AbstractPathIntegrator() : 

	Integrator(),

	m_scene      (nullptr),
	m_camera     (nullptr),
	m_sg(nullptr),
	m_domainPx   (),
	m_widthPx(0), m_heightPx(0),
	m_filter     (SampleFilterFactory::createGaussianFilter()),
	m_filmMutex(),

	m_lightEnergy(nullptr)
{}

AbstractPathIntegrator::AbstractPathIntegrator(const AbstractPathIntegrator& other) : 

	Integrator(other),

	m_scene      (other.m_scene),
	m_camera     (other.m_camera),
	m_sg(other.m_sg),
	m_domainPx   (other.m_domainPx),
	m_widthPx(other.m_widthPx), m_heightPx(other.m_heightPx),
	m_filter     (other.m_filter),
	m_filmMutex(),

	m_lightEnergy(nullptr)
{}

AbstractPathIntegrator::~AbstractPathIntegrator() = default;

AttributeTags AbstractPathIntegrator::supportedAttributes() const
{
	AttributeTags supports;
	supports.tag(EAttribute::LIGHT_ENERGY);
	return supports;
}

void AbstractPathIntegrator::setDomainPx(const TAABB2D<int64>& domain, uint32 widthPx, uint32 heightPx)
{
	m_domainPx = domain;
	m_widthPx = widthPx;
	m_heightPx = heightPx;
}

void AbstractPathIntegrator::setIntegrand(const RenderWork& integrand)
{
	m_scene = integrand.scene;
	m_camera = integrand.camera;
	m_sg = integrand.sampleGenerator;
}

void AbstractPathIntegrator::integrate(const AttributeTags& requestedAttributes)
{
	if(!initFilms())
	{
		std::cerr << "film init failed" << std::endl;
		return;
	}

	const Camera* const         camera     = m_camera;
	const Scene* const          scene      = m_scene;
	SampleGenerator* const      sg         = m_sg;
	SpectralSamplingFilm* const film       = m_lightEnergy.get();

	const uint64 filmWpx = film->getEffectiveResPx().x;
	const uint64 filmHpx = film->getEffectiveResPx().y;

	const Vector2D flooredSampleMinVertex = film->getSampleWindowPx().minVertex.floor();
	const Vector2D ceiledSampleMaxVertex  = film->getSampleWindowPx().maxVertex.ceil();
	const uint64 filmSampleWpx = static_cast<uint64>(ceiledSampleMaxVertex.x - flooredSampleMinVertex.x);
	const uint64 filmSampleHpx = static_cast<uint64>(ceiledSampleMaxVertex.y - flooredSampleMinVertex.y);
	const uint64 numCamPhaseSamples = filmSampleWpx * filmSampleHpx;

	TSamplePhase<SampleArray2D> camSamplePhase = sg->declareArray2DPhase(numCamPhaseSamples);

	m_statistics.setTotalWork(static_cast<uint32>(sg->numSamples()));
	m_statistics.setWorkDone(0);

	std::chrono::time_point<std::chrono::system_clock> t1;
	std::chrono::time_point<std::chrono::system_clock> t2;

	while(sg->singleSampleStart())
	{
		t1 = std::chrono::system_clock::now();

		const SampleArray2D& camSamples = sg->getNextArray2D(camSamplePhase);

		for(std::size_t si = 0; si < camSamples.numElements(); si++)
		{
			const Vector2D rasterPosPx(camSamples[si].x * filmSampleWpx + flooredSampleMinVertex.x,
			                           camSamples[si].y * filmSampleHpx + flooredSampleMinVertex.y);

			if(!film->getSampleWindowPx().isIntersectingArea(rasterPosPx))
			{
				continue;
			}

			Ray ray;
			camera->genSensedRay(Vector2R(rasterPosPx), &ray);

			SurfaceHit firstHit;
			SpectralStrength lightEnergySample;
			tracePath(ray, &lightEnergySample, &firstHit);

			film->addSample(rasterPosPx.x, rasterPosPx.y, lightEnergySample);
		}// end for

		sg->singleSampleEnd();

		//m_renderer.submitWork(m_id, work, true);
		m_statistics.incrementWorkDone();
	
		t2 = std::chrono::system_clock::now();

		m_statistics.setNumSamplesTaken(static_cast<uint32>(camSamples.numElements()));
		m_statistics.setNumMsElapsed(static_cast<uint32>(std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count()));
	}

	// FIXME: sort of hacking
	m_statistics.setNumSamplesTaken(0);
	m_statistics.setNumMsElapsed(0);
}

void AbstractPathIntegrator::asyncGetAttribute(const EAttribute target, HdrRgbFrame& out_frame)
{
	std::lock_guard<std::mutex> lock(m_filmMutex);

	switch(target)
	{
	case EAttribute::LIGHT_ENERGY:
		m_lightEnergy->develop(out_frame);
		break;
	}
}

void swap(AbstractPathIntegrator& first, AbstractPathIntegrator& second)
{
	using std::swap;

	swap(static_cast<Integrator&>(first), static_cast<Integrator&>(second));

	swap(first.m_scene,       second.m_scene);
	swap(first.m_camera,      second.m_camera);
	swap(first.m_sg, second.m_sg);
	swap(first.m_domainPx,    second.m_domainPx);
	swap(first.m_filter,      second.m_filter);
	swap(first.m_widthPx, second.m_widthPx);
	swap(first.m_heightPx, second.m_heightPx);
	// mutexes are not swapped

	swap(first.m_lightEnergy, second.m_lightEnergy);
}

bool AbstractPathIntegrator::initFilms()
{
	std::lock_guard<std::mutex> lock(m_filmMutex);

	if(!m_domainPx.isValid() || m_domainPx.calcArea() == 0)
	{
		std::cerr << "integration domain invalid: " << m_domainPx.toString() << std::endl;
		return false;
	}

	// FIXME: filter should not be shared_ptr
	m_lightEnergy = std::make_unique<HdrRgbFilm>(m_widthPx, m_heightPx, m_domainPx, std::make_shared<SampleFilter>(m_filter));

	return true;
}

// command interface

AbstractPathIntegrator::AbstractPathIntegrator(const InputPacket& packet) :

	Integrator(packet),

	m_scene(nullptr),
	m_camera(nullptr),
	m_sg(nullptr),
	m_domainPx(),
	m_widthPx(0), m_heightPx(0),
	m_filter(SampleFilterFactory::createGaussianFilter()),
	m_filmMutex(),

	m_lightEnergy(nullptr)
{
	const std::string filterName = packet.getString("filter-name", "box");

	if(filterName == "box")
	{
		m_filter = SampleFilterFactory::createBoxFilter();
	}
	else if(filterName == "gaussian")
	{
		m_filter = SampleFilterFactory::createGaussianFilter();
	}
	else if(filterName == "mn")
	{
		m_filter = SampleFilterFactory::createMNFilter();
	}
}

SdlTypeInfo AbstractPathIntegrator::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_INTEGRATOR, "abstract-path-integrator");
}

void AbstractPathIntegrator::ciRegister(CommandRegister& cmdRegister)
{}

}// end namespace ph