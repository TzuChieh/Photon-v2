#pragma once

#include "Core/VolumeBehavior/VolumeOptics.h"
#include "Core/Texture/texture_fwd.h"

#include <memory>

namespace ph
{

class HenyeyGreenstein final : public VolumeOptics
{
public:
	HenyeyGreenstein(real g);
	HenyeyGreenstein(const std::shared_ptr<TTexture<real>>& g);
	~HenyeyGreenstein() override;

	void evalPhaseFunction(
		const SurfaceHit& X,
		const Vector3R&   I,
		const Vector3R&   O,
		real*             out_pf) const override;

private:
	std::shared_ptr<TTexture<real>> m_g;
};

}// end namespace ph