#pragma once

#include "Actor/PhysicalActor.h"
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
class ADome : public PhysicalActor
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
