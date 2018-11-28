#pragma once

#include "Actor/PhysicalActor.h"
#include "Common/Logger.h"
#include "FileIO/FileSystem/Path.h"

namespace ph
{

/*
	An actor that represents the sky of the scene.
*/
class ADome final : public PhysicalActor, public TCommandInterface<ADome>
{
public:
	ADome();
	ADome(const Path& envMap);
	ADome(const ADome& other);
	virtual ~ADome() override;

	virtual CookedUnit cook(CookingContext& context) const override;

	ADome& operator = (ADome rhs);

	friend void swap(ADome& first, ADome& second);

private:
	Path m_sphericalEnvMap;

	static const Logger logger;

// command interface
public:
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
	static std::unique_ptr<ADome> ciLoad(const InputPacket& packet);
};

}// end namespace ph

/*
	<SDL_interface>

	<category>  actor          </category>
	<type_name> dome           </type_name>
	<extend>    actor.physical </extend>

	<name> Dome Actor </name>
	<description>
		A large energy emitting source encompassing the whole scene.
	</description>

	<command type="creator">
		<input name="env-map" type="string">
			<description>
				Resource identifier for a HDRI describing the energy distribution.
			</description>
		</input>
	</command>

	</SDL_interface>
*/