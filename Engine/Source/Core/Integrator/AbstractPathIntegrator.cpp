#include "Core/Integrator/AbstractPathIntegrator.h"
#include "Common/assertion.h"

namespace ph
{

AbstractPathIntegrator::AbstractPathIntegrator() = default;

AbstractPathIntegrator::AbstractPathIntegrator(const AbstractPathIntegrator& other)
{

}

AbstractPathIntegrator::~AbstractPathIntegrator() = default;

void AbstractPathIntegrator::setDomainPx(const TAABB2D<int64>& domain)
{

}

void AbstractPathIntegrator::setIntegrand(const RenderWork& integrand)
{

}

void AbstractPathIntegrator::integrate(const std::vector<EAttribute>& targetTypes)
{

}

void AbstractPathIntegrator::asyncGetDomainAttribute(const EAttribute target, HdrRgbFrame& out_frame)
{

}

AbstractPathIntegrator& AbstractPathIntegrator::operator = (const AbstractPathIntegrator& rhs)
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

void AbstractPathIntegrator::initFilms()
{

}

}// end namespace ph