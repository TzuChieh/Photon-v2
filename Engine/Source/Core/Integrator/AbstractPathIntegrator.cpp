#include "Core/Integrator/AbstractPathIntegrator.h"
#include "Common/assertion.h"
#include "Core/Filmic/HdrRgbFilm.h"
#include "Core/Filmic/SampleFilterFactory.h"
#include "Core/SampleGenerator/SampleGenerator.h"
#include "Core/Camera/Camera.h"
#include "World/Scene.h"

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

	// TODO
}

void AbstractPathIntegrator::asyncGetAttribute(const EAttribute target, HdrRgbFrame& out_frame)
{
	std::lock_guard<std::mutex> lock(m_filmMutex);

	// TODO
}

void swap(AbstractPathIntegrator& first, AbstractPathIntegrator& second)
{
	using std::swap;

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
		return;
	}

	// FIXME: filter should not be shared_ptr
	m_lightEnergy = std::make_unique<HdrRgbFilm>(m_widthPx, m_heightPx, std::make_shared<SampleFilter>(m_filter));
}

// command interface

AbstractPathIntegrator::AbstractPathIntegrator(const InputPacket& packet) :
	AbstractPathIntegrator(packet)
{}

SdlTypeInfo AbstractPathIntegrator::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_INTEGRATOR, "abstract-path-integrator");
}

void AbstractPathIntegrator::ciRegister(CommandRegister& cmdRegister)
{
	// TODO
}

}// end namespace ph