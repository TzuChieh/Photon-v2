#include "Actor/LightSource/RectangleSource.h"
#include "Common/assertion.h"
#include "Actor/Geometry/GRectangle.h"

#include <memory>

namespace ph
{
	
RectangleSource::RectangleSource() : 
	RectangleSource(1, 1, Vector3R(1, 1, 1), 100)
{}

RectangleSource::RectangleSource(
	const real      width, 
	const real      height, 
	const Vector3R& linearSrgbColor, 
	const real      numWatts) : 

	AreaSource(linearSrgbColor, numWatts)
{
	setDimension(width, height);
}

RectangleSource::RectangleSource(
	const real                     width, 
	const real                     height, 
	const SampledSpectralStrength& color, 
	const real                     numWatts) : 

	AreaSource(color, numWatts)
{
	setDimension(width, height);
}

RectangleSource::~RectangleSource() = default;

std::vector<std::unique_ptr<Geometry>> RectangleSource::genAreas() const
{
	std::vector<std::unique_ptr<Geometry>> areas;
	areas.push_back(std::make_unique<GRectangle>(m_width, m_height));
	return std::move(areas);
}

void RectangleSource::setDimension(const real width, const real height)
{
	PH_ASSERT(width > 0.0_r && height > 0.0_r);

	m_width  = width;
	m_height = height;
}

// command interface

RectangleSource::RectangleSource(const InputPacket& packet) : 
	AreaSource(packet)
{
	const real width  = packet.getReal("width");
	const real height = packet.getReal("height");
	setDimension(width, height);
}

SdlTypeInfo RectangleSource::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_LIGHT_SOURCE, "rectangle");
}

void RectangleSource::ciRegister(CommandRegister& cmdRegister)
{
	cmdRegister.setLoader(SdlLoader([](const InputPacket& packet)
	{
		return std::make_unique<RectangleSource>(packet);
	}));
}

}// end namespace ph