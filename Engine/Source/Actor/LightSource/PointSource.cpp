#include "Actor/LightSource/PointSource.h"

namespace ph
{

namespace
{
	static constexpr real POINT_SOURCE_RADIUS = 0.005_r;
}

PointSource::PointSource() : 
	PointSource(Vector3R(1, 1, 1), 100.0_r)
{}

PointSource::PointSource(const Vector3R& linearSrgbColor, const real numWatts) : 
	m_sphereSource(POINT_SOURCE_RADIUS, linearSrgbColor, numWatts)
{}

PointSource::PointSource(const SampledSpectralStrength& color, real numWatts) : 
	m_sphereSource(POINT_SOURCE_RADIUS, color, numWatts)
{}

PointSource::~PointSource() = default;

std::shared_ptr<Geometry> PointSource::genAreas(CookingContext& context) const
{
	return m_sphereSource.genAreas(context);
}

// command interface

PointSource::PointSource(const InputPacket& packet) : 
	AreaSource(packet)
{
	m_sphereSource.setRadius(POINT_SOURCE_RADIUS);
}

SdlTypeInfo PointSource::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_LIGHT_SOURCE, "point");
}

void PointSource::ciRegister(CommandRegister& cmdRegister)
{
	cmdRegister.setLoader(SdlLoader([](const InputPacket& packet)
	{
		return std::make_unique<PointSource>(packet);
	}));
}

}// end namespace ph