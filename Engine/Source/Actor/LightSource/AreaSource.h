#pragma once

#include "Actor/LightSource/LightSource.h"
#include "Math/math_fwd.h"

#include <memory>
#include <string>

namespace ph
{

class Texture;

class AreaSource final : public LightSource
{
public:
	AreaSource(const Vector3R& emittedRadiance);
	AreaSource(const std::string& imageFilename);
	AreaSource(const InputPacket& packet);
	virtual ~AreaSource() override;

	virtual std::unique_ptr<Emitter> buildEmitter(const EmitterBuildingMaterial& data) const override;

private:
	std::shared_ptr<Texture> m_emittedRadiance;
};

}// end namespace ph