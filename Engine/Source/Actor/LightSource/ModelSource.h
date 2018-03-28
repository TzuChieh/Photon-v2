#pragma once

#include "Actor/LightSource/LightSource.h"
#include "Math/math_fwd.h"
#include "FileIO/SDL/TCommandInterface.h"
#include "FileIO/FileSystem/Path.h"
#include "Common/Logger.h"

#include <memory>
#include <string>

namespace ph
{

class Image;

class ModelSource final : public LightSource, public TCommandInterface<ModelSource>
{
public:
	ModelSource(const Vector3R& emittedRgbRadiance);
	ModelSource(const Path& imagePath);
	ModelSource(const std::shared_ptr<Image>& emittedRadiance);
	virtual ~ModelSource() override;

	virtual std::unique_ptr<Emitter> genEmitter(
		CookingContext& context, EmitterBuildingMaterial&& data) const override;

private:
	std::shared_ptr<Image> m_emittedRadiance;

	static const Logger logger;

// command interface
public:
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
	static std::unique_ptr<ModelSource> ciLoad(const InputPacket& packet);
};

}// end namespace ph