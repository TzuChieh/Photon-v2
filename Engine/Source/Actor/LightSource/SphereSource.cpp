#include "Actor/LightSource/SphereSource.h"
#include "Actor/Geometry/GSphere.h"

namespace ph
{

SphereSource::SphereSource() :
	AreaSource()
{
	setRadius(1.0_r);
}

SphereSource::SphereSource(const real radius, const Vector3R& linearSrgbColor, real numWatts) : 
	AreaSource(linearSrgbColor, numWatts)
{
	setRadius(radius);
}

SphereSource::SphereSource(const real radius, const SampledSpectralStrength& color, real numWatts) : 
	AreaSource(color, numWatts)
{
	setRadius(radius);
}

SphereSource::~SphereSource() = default;

std::vector<std::unique_ptr<Geometry>> SphereSource::genAreas() const
{
	std::vector<std::unique_ptr<Geometry>> areas;
	areas.push_back(std::make_unique<GSphere>(m_radius));
	return std::move(areas);
}

void SphereSource::setRadius(const real radius)
{
	PH_ASSERT(radius > 0.0_r);

	m_radius = radius;
}

// command interface

SphereSource::SphereSource(const InputPacket& packet) : 
	AreaSource(packet),
	m_radius(1.0_r)
{
	m_radius = packet.getReal("radius");
}

SdlTypeInfo SphereSource::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_LIGHT_SOURCE, "sphere");
}

void SphereSource::ciRegister(CommandRegister& cmdRegister)
{
	cmdRegister.setLoader(SdlLoader([](const InputPacket& packet)
	{
		return std::make_unique<SphereSource>(packet);
	}));
}

}// end namespace ph