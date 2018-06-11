#include "Core/Integrator/AbstractPathIntegrator.h"
#include "Common/assertion.h"
#include "Core/Filmic/HdrRgbFilm.h"

#include <iostream>

namespace ph
{

AbstractPathIntegrator::AbstractPathIntegrator() : 

	Integrator(),

	m_scene      (nullptr),
	m_camera     (nullptr),
	m_domainPx   (),

	m_lightEnergy(nullptr)
{}

AbstractPathIntegrator::AbstractPathIntegrator(const AbstractPathIntegrator& other) : 

	Integrator(other),

	m_scene      (other.m_scene),
	m_camera     (other.m_camera),
	m_domainPx   (other.m_domainPx),

	m_lightEnergy(nullptr)
{}

AbstractPathIntegrator::~AbstractPathIntegrator() = default;

AttributeTags AbstractPathIntegrator::supportedAttributes() const
{
	AttributeTags supports;
	supports.tag(EAttribute::LIGHT_ENERGY);
	return supports;
}

void AbstractPathIntegrator::setDomainPx(const TAABB2D<int64>& domain)
{

}

void AbstractPathIntegrator::setIntegrand(const RenderWork& integrand)
{

}

void AbstractPathIntegrator::integrate(const AttributeTags& requestedAttributes)
{

}

void AbstractPathIntegrator::asyncGetAttribute(const EAttribute target, HdrRgbFrame& out_frame)
{

}

void swap(AbstractPathIntegrator& first, AbstractPathIntegrator& second)
{
	using std::swap;

	swap(first.m_scene,       second.m_scene);
	swap(first.m_camera,      second.m_camera);
	swap(first.m_domainPx,    second.m_domainPx);
	swap(first.m_lightEnergy, second.m_lightEnergy);
}

bool AbstractPathIntegrator::initFilms()
{
	if(!m_domainPx.isValid() || m_domainPx.calcArea() == 0)
	{
		std::cerr << "integration domain invalid: " << m_domainPx.toString() << std::endl;
		return;
	}

	// TODO: film dimension & filter
	m_lightEnergy = std::make_unique<HdrRgbFilm>();
}

}// end namespace ph