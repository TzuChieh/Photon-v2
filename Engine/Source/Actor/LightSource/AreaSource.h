#pragma once

#include "Actor/LightSource/LightSource.h"
#include "Core/Quantity/SpectralStrength.h"
#include "FileIO/InputPacket.h"
#include "Actor/Geometry/Geometry.h"

#include <memory>

namespace ph
{

class Image;

class AreaSource : public LightSource, public TCommandInterface<AreaSource>
{
public:
	AreaSource();
	AreaSource(const Vector3R& linearSrgbColor, real numWatts);
	AreaSource(const SampledSpectralStrength& color, real numWatts);
	virtual ~AreaSource() override;

	virtual std::shared_ptr<Geometry> genAreas(CookingContext& context) const = 0;

	virtual std::unique_ptr<Emitter> genEmitter(
		CookingContext& context, EmitterBuildingMaterial&& data) const override;

	virtual std::shared_ptr<Geometry> genGeometry(CookingContext& context) const final override;

private:
	SampledSpectralStrength m_color;
	real                    m_numWatts;

// command interface
public:
	AreaSource(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
};

}// end namespace ph