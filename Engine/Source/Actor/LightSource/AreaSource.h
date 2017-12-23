#pragma once

#include "Actor/LightSource/LightSource.h"
#include "Math/math_fwd.h"
#include "FileIO/SDL/TCommandInterface.h"

#include <memory>
#include <string>

namespace ph
{
class Image;

class AreaSource final : public LightSource, public TCommandInterface<AreaSource>
{
public:
	AreaSource(const Vector3R& emittedRgbRadiance);
	AreaSource(const std::string& imageFilename);
	AreaSource(const std::shared_ptr<Image>& emittedRadiance);
	virtual ~AreaSource() override;

	virtual std::unique_ptr<Emitter> genEmitter(
		CookingContext& context, const EmitterBuildingMaterial& data) const override;

private:
	std::shared_ptr<Image> m_emittedRadiance;

// command interface
public:
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
	static std::unique_ptr<AreaSource> ciLoad(const InputPacket& packet);
};

}// end namespace ph