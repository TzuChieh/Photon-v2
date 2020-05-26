#pragma once

#include "Actor/AModel.h"
#include "DataIO/SDL/TCommandInterface.h"

#include <string>

namespace ph
{

class APhantomModel : public AModel, public TCommandInterface<APhantomModel>
{
public:
	APhantomModel();
	APhantomModel(
		const std::shared_ptr<Geometry>& geometry,
		const std::shared_ptr<Material>& material);
	APhantomModel(const APhantomModel& other);

	CookedUnit cook(CookingContext& context) override;
	CookOrder getCookOrder() const override;

	APhantomModel& operator = (APhantomModel rhs);

	friend void swap(APhantomModel& first, APhantomModel& second);

private:
	std::string m_phantomName;

// command interface
public:
	explicit APhantomModel(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
};

// In-header Implementations:

inline CookOrder APhantomModel::getCookOrder() const
{
	return CookOrder(ECookPriority::HIGH);
}

}// end namespace ph

/*
	<SDL_interface>

	<category>  actor         </category>
	<type_name> phantom-model </type_name>
	<extend>    actor.model   </extend>

	<name> Phantom Model Actor </name>
	<description>
		An actor that itself will not appear in the scene, but its cooked
		result can be referenced by others.
	</description>

	<command type="creator">
		<input name="name" type="string">
			<description>Phantom's name.</description>
		</input>
	</command>

	</SDL_interface>
*/
