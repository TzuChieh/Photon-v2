#pragma once

#include "Actor/ADome.h"
#include "Common/primitive_type.h"
#include "DataIO/FileSystem/Path.h"
#include "Core/Texture/TTexture.h"
#include "Core/Quantity/Spectrum.h"

#include <memory>

namespace ph
{

/*! @brief Model the sky of the scene with an image.

Using a background image to represent the energy emitted from far away.
*/
class APreethamDome : public ADome, public TCommandInterface<APreethamDome>
{
public:
	APreethamDome();

	APreethamDome(
		real sunPhiDegrees,
		real sunThetaDegrees,
		real turbidity);

	APreethamDome(const APreethamDome& other);

	std::shared_ptr<TTexture<Spectrum>> loadRadianceFunction(CookingContext& context) override;
	math::Vector2S getResolution() const override;

	APreethamDome& operator = (APreethamDome rhs);

	friend void swap(APreethamDome& first, APreethamDome& second);

private:
	real m_sunPhi;
	real m_sunTheta;
	real m_turbidity;

	static void checkTurbidity(real turbidity);

// command interface
public:
	explicit APreethamDome(const InputPacket& packet);
};

// In-header Implementations:

inline math::Vector2S APreethamDome::getResolution() const
{
	return {0, 0};
}

}// end namespace ph
