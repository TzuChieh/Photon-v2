#pragma once

#include "Actor/LightSource/LightSource.h"
#include "Core/Quantity/SpectralStrength.h"

#include <memory>

namespace ph
{

class Image;

class PointSource : public LightSource, public TCommandInterface<PointSource>
{
public:
	PointSource();
	PointSource(const Vector3R& linearSrgbColor, real numWatts);
	PointSource(const SampledSpectralStrength& color, real numWatts);
	virtual ~PointSource() override;

	virtual std::unique_ptr<Emitter> genEmitter(
		CookingContext& context, EmitterBuildingMaterial&& data) const override;

private:
	SampledSpectralStrength m_color;
	real                    m_numWatts;

// command interface
public:
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
	static std::unique_ptr<PointSource> loadPointSource(const InputPacket& packet);
};

}// end namespace ph