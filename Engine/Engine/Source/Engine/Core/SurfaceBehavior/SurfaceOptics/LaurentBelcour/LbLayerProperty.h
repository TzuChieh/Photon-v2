#pragma once

#include "Engine/Core/SurfaceBehavior/SurfaceOptics/LaurentBelcour/LbLayer.h"

namespace ph
{

class SurfaceHit;

/*! @brief Provides the physical properties of a single Laurent Belcour layer.
*/
class LbLayerProperty
{
public:
	virtual ~LbLayerProperty() = default;

	virtual LbLayer evaluate(
		const SurfaceHit& X,
		const LbLayer& previousLayer) const = 0;
};

}// end namespace ph
