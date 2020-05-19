#pragma once

#include "Actor/PhysicalActor.h"
#include "Common/Logger.h"
#include "DataIO/FileSystem/Path.h"
#include "Core/Texture/TTexture.h"
#include "Core/Quantity/Spectrum.h"

#include <memory>

namespace ph
{

/*! @brief An actor that models the sky of the scene.

Model the sky in latitude-longitude format. Effectively a large energy
emitting source encompassing the whole scene.
*/
class ADome : public PhysicalActor, public TCommandInterface<ADome>
{
public:
	ADome();
	explicit ADome(const Path& envMap);
	ADome(const ADome& other);

	CookedUnit cook(CookingContext& context) const override;
	CookOrder getCookOrder() const override;

	ADome& operator = (ADome rhs);

	friend void swap(ADome& first, ADome& second);

private:
	Path m_sphericalEnvMap;

	static std::shared_ptr<TTexture<Spectrum>> loadRadianceTexture(
		const Path&     filePath, 
		CookingContext& context, 
		math::Vector2S* out_resolution = nullptr);

// command interface
public:
	explicit ADome(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
};

// In-header Implementations:

inline CookOrder ADome::getCookOrder() const
{
	return CookOrder(ECookPriority::LOW, ECookLevel::LAST);
}

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
