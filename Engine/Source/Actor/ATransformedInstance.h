#pragma once

#include "Actor/PhysicalActor.h"
#include "FileIO/SDL/TCommandInterface.h"

#include <string>

namespace ph
{

class ATransformedInstance : public PhysicalActor, public TCommandInterface<ATransformedInstance>
{
public:
	ATransformedInstance();
	ATransformedInstance(const ATransformedInstance& other);

	CookedUnit cook(CookingContext& context) const override;

	ATransformedInstance& operator = (ATransformedInstance rhs);

	friend void swap(ATransformedInstance& first, ATransformedInstance& second);

private:
	std::string m_phantomName;

// command interface
public:
	explicit ATransformedInstance(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
};

}// end namespace ph

/*
	<SDL_interface>

	<category>  actor                </category>
	<type_name> transformed-instance </type_name>
	<extend>    actor.physical       </extend>

	<name> Transformed Instance </name>
	<description>
		An actor that applies a transformation effect on a phantom.
	</description>

	<command type="creator">
		<input name="name" type="string">
			<description>Target phantom's name.</description>
		</input>
	</command>

	</SDL_interface>
*/