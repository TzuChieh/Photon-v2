#pragma once

#include "Actor/LightSource/LightSource.h"

#include <memory>

namespace ph
{

class Vector3f;
class Texture;

class AreaSource final : public LightSource
{
public:
	AreaSource(const Vector3f& emittedRadiance);
	virtual ~AreaSource() override;

	virtual void buildEmitter(CoreActor& coreActor) const override;

private:
	std::shared_ptr<Texture> m_emittedRadiance;

	static bool checkCoreActor(const CoreActor& coreActor);
};

}// end namespace ph