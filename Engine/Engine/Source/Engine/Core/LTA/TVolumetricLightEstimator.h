#pragma once

#include "Engine/Math/math_fwd.h"
#include "Engine/Math/Color/Spectrum.h"
#include "Engine/Core/LTA/SidednessAgreement.h"

namespace ph { class Scene; }
namespace ph { class SurfaceHit; }

namespace ph::lta
{

template<ESidednessPolicy POLICY>
class TVolumetricLightEstimator final
{
public:
	explicit TVolumetricLightEstimator(const Scene* scene);

	[[nodiscard]]
	bool sample(
		const SurfaceHit& Xs,
		const math::Vector3R& L,
		SurfaceHit* out_Xe,
		math::Vector3R* out_V,
		math::Spectrum* out_weight,
		math::Spectrum* out_radiance);

private:
	const Scene& getScene() const;

	const Scene* m_scene;
};

}// end namespace ph::lta

#include "Engine/Core/LTA/TVolumetricLightEstimator.ipp"
