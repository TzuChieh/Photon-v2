#pragma once

#include "Actor/LightSource/AreaSource.h"

namespace ph
{

class RectangleSource final : public AreaSource, public TCommandInterface<RectangleSource>
{
public:
	RectangleSource();
	RectangleSource(real width, real height, const Vector3R& linearSrgbColor, real numWatts);
	RectangleSource(real width, real height, const SampledSpectralStrength& color, real numWatts);
	virtual ~RectangleSource() override;

	virtual std::shared_ptr<Geometry> genAreas(CookingContext& context) const override;

	void setDimension(real width, real height);

private:
	real m_width;
	real m_height;

// command interface
public:
	RectangleSource(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
};

}// end namespace ph