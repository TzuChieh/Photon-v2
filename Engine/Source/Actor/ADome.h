#pragma once

#include "Actor/PhysicalActor.h"
#include "Common/Logger.h"
#include "FileIO/FileSystem/Path.h"

namespace ph
{

/*
	An actor that represents the sky of the scene.
*/
class ADome : public PhysicalActor, public TCommandInterface<ADome>
{
public:
	ADome();
	explicit ADome(const Path& envMap);
	ADome(const ADome& other);

	CookedUnit cook(CookingContext& context) const override;

	ADome& operator = (ADome rhs);

	friend void swap(ADome& first, ADome& second);

private:
	Path m_sphericalEnvMap;

	static const Logger logger;

// command interface
public:
	explicit ADome(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
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