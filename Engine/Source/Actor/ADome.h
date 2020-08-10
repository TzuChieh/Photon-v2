#pragma once

#include "Actor/PhysicalActor.h"
#include "Common/Logger.h"
#include "DataIO/FileSystem/Path.h"
#include "Core/Texture/TTexture.h"
#include "Core/Quantity/Spectrum.h"
#include "Math/TVector2.h"

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
	ADome(const ADome& other);

	virtual std::shared_ptr<TTexture<Spectrum>> loadRadianceFunction(CookingContext& context) = 0;
	virtual math::Vector2S getResolution() const = 0;

	CookedUnit cook(CookingContext& context) override;
	CookOrder getCookOrder() const override;

	bool isAnalytical() const;

	ADome& operator = (const ADome& rhs);

	friend void swap(ADome& first, ADome& second);

private:
	real m_energyScale;

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

inline bool ADome::isAnalytical() const
{
	const auto resolution = getResolution();
	return resolution.x == 0 && resolution.y == 0;
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
		<input name="energy-scale" type="real">
			<description>
				A non-physical scale factor for artistic purpose.
			</description>
		</input>
	</command>

	</SDL_interface>
*/
