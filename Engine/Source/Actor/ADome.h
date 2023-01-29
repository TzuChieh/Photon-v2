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

struct DomeRadianceFunctionInfo final
{
	math::Vector2S resolution = {1, 1};
	bool isAnalytical = false;
};

/*! @brief An actor that models the sky of the scene.
Model the sky in latitude-longitude format. Effectively a large energy emitting source encompassing 
the whole scene.
*/
class ADome : public PhysicalActor
{
public:
	virtual std::shared_ptr<TTexture<math::Spectrum>> loadRadianceFunction(
		CookingContext& ctx, DomeRadianceFunctionInfo* out_info) = 0;

	CookedUnit cook(CookingContext& ctx, const PreCookReport& report) override;
	CookOrder getCookOrder() const override;

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

}// end namespace ph
