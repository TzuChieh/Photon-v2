#pragma once

#include "Actor/PhysicalActor.h"
#include "DataIO/FileSystem/Path.h"
#include "Core/Texture/TTexture.h"
#include "Math/Color/Spectrum.h"
#include "Math/TVector2.h"
#include "DataIO/SDL/sdl_interface.h"

#include <memory>

namespace ph
{

/*! @brief An actor that models the sky of the scene.

Model the sky in latitude-longitude format. Effectively a large energy
emitting source encompassing the whole scene.
*/
class ADome : public PhysicalActor
{
public:
	ADome();
	ADome(const ADome& other) = default;

	virtual std::shared_ptr<TTexture<math::Spectrum>> loadRadianceFunction(ActorCookingContext& context) = 0;
	virtual math::Vector2S getResolution() const = 0;

	CookedUnit cook(ActorCookingContext& ctx) override;
	CookOrder getCookOrder() const override;

	bool isAnalytical() const;

	ADome& operator = (const ADome& rhs);

	friend void swap(ADome& first, ADome& second);

private:
	real m_energyScale;

public:
	PH_DEFINE_SDL_CLASS(TOwnerSdlClass<ADome>)
	{
		ClassType clazz("dome");
		clazz.docName("Dome Actor");
		clazz.description("A large energy emitting source encompassing the whole scene.");
		clazz.baseOn<PhysicalActor>();

		TSdlReal<OwnerType> energyScale("energy-scale", &OwnerType::m_energyScale);
		energyScale.description("A non-physical scale factor for artistic purpose.");
		energyScale.defaultTo(1);
		energyScale.optional();
		clazz.addField(energyScale);

		return clazz;
	}
};

// In-header Implementations:

inline CookOrder ADome::getCookOrder() const
{
	return CookOrder(ECookPriority::LOW, ECookLevel::LAST);
}

inline bool ADome::isAnalytical() const
{
	const auto resolution = getResolution();
	return resolution.x() == 0 && resolution.y() == 0;
}

}// end namespace ph
