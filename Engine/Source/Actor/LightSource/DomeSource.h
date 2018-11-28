#pragma once

#include "Actor/LightSource/EmitterBuildingMaterial.h"
#include "FileIO/SDL/TCommandInterface.h"
#include "Actor/LightSource/LightSource.h"
#include "Common/Logger.h"
#include "FileIO/FileSystem/Path.h"

#include <memory>

namespace ph
{

class DomeSource final : public LightSource, public TCommandInterface<DomeSource>
{
public:
	DomeSource();
	DomeSource(const Path& sphericalEnvMap);
	virtual ~DomeSource() override;

	virtual std::unique_ptr<Emitter> genEmitter(
		CookingContext& context, EmitterBuildingMaterial&& data) const override;

	virtual std::shared_ptr<Geometry> genGeometry(CookingContext& context) const override;

	virtual std::shared_ptr<Material> genMaterial(CookingContext& context) const override;

private:
	Path m_sphericalEnvMap;

// command interface
public:
	DomeSource(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);

	static const Logger logger;
};

}// end namespace ph

/*
	<SDL_interface>

	<category>  light-source              </category>
	<type_name> dome                      </type_name>
	<extend>    light-source.light-source </extend>

	<name> Dome Source </name>
	<description>
		A large energy emitting source encompassing the whole scene.
	</description>

	<command type="creator"/>

	</SDL_interface>
*/