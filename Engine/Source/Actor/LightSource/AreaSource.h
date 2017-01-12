#pragma once

#include "Actor/LightSource/LightSource.h"

#include <memory>
#include <string>

namespace ph
{

class Vector3f;
class Texture;

class AreaSource final : public LightSource
{
public:
	AreaSource(const Vector3f& emittedRadiance);
	AreaSource(const std::string& imageFilename);
	virtual ~AreaSource() override;

	virtual void buildEmitter(CoreActor& coreActor) const override;

private:
	std::shared_ptr<Texture> m_emittedRadiance;

	static bool checkCoreActor(const CoreActor& coreActor);
};

}// end namespace ph