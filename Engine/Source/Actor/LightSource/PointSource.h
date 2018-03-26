#pragma once

#include "Actor/LightSource/LightSource.h"

#include <memory>

namespace ph
{

class Image;

class PointSource : public LightSource, public TCommandInterface<PointSource>
{
public:
	PointSource();
	PointSource(const Vector3R& linearSrgbRadiance);
	PointSource(const std::shared_ptr<Image> radiance);
	virtual ~PointSource() override;

	virtual std::unique_ptr<Emitter> genEmitter(
		CookingContext& context, EmitterBuildingMaterial&& data) const override;

private:
	std::shared_ptr<Image> m_emittedRadiance;

// command interface
public:
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
	static std::unique_ptr<PointSource> loadPointSource(const InputPacket& packet);
};

}// end namespace ph