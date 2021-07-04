#pragma once

#include "DataIO/Option.h"
#include "DataIO/SDL/sdl_interface.h"

namespace ph
{

enum class EAccelerator
{
	BRUTE_FORCE,
	BVH,
	KDTREE,
	INDEXED_KDTREE
};

class CookSettings final : public Option
{
public:
	CookSettings();
	explicit CookSettings(EAccelerator topLevelAccelerator);

	void setTopLevelAccelerator(EAccelerator accelerator);
	EAccelerator getTopLevelAccelerator() const;

private:
	EAccelerator m_topLevelAccelerator;

public:
	PH_DEFINE_SDL_CLASS(TOwnerSdlClass<CookSettings>)
	{
		ClassType clazz("cook-settings");
		clazz.description(
			"Settings related to the actor-cooking process.");
		clazz.baseOn<Option>();

		TSdlSpectrum<OwnerType> color("color", EQuantity::EMR, &OwnerType::m_color);
		color.description("The color of this light source.");
		color.defaultTo(Spectrum().setLinearSrgb({ 1, 1, 1 }, EQuantity::EMR));
		clazz.addField(color);

		TSdlReal<OwnerType> numWatts("watts", &OwnerType::m_numWatts);
		numWatts.description("Energy emitted by this light source, in watts.");
		numWatts.defaultTo(100);
		clazz.addField(numWatts);

		return clazz;
	}
};

// In-header Implementations:

inline void CookSettings::setTopLevelAccelerator(const EAccelerator accelerator)
{
	m_topLevelAccelerator = accelerator;
}

inline EAccelerator CookSettings::getTopLevelAccelerator() const
{
	return m_topLevelAccelerator;
}

}// end namespace ph
