#pragma once

#include "Actor/LightSource/LightSource.h"
#include "Math/math_fwd.h"
#include "FileIO/SDL/TCommandInterface.h"

#include <memory>
#include <string>

namespace ph
{

class Texture;

class AreaSource final : public LightSource, public TCommandInterface<AreaSource>
{
public:
	AreaSource(const Vector3R& emittedRadiance);
	AreaSource(const std::string& imageFilename);
	virtual ~AreaSource() override;

	virtual std::unique_ptr<Emitter> buildEmitter(const EmitterBuildingMaterial& data) const override;

private:
	std::shared_ptr<Texture> m_emittedRadiance;

// command interface
public:
	AreaSource(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static ExitStatus ciExecute(const std::shared_ptr<AreaSource>& targetResource, const std::string& functionName, const InputPacket& packet);
};

}// end namespace ph