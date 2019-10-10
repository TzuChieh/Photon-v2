#pragma once

#include "Core/VolumeBehavior/ScatterFunction.h"
#include "Core/Texture/texture_fwd.h"

#include <memory>

namespace ph
{

class HenyeyGreenstein : public ScatterFunction
{
public:
	HenyeyGreenstein(real g);
	HenyeyGreenstein(const std::shared_ptr<TTexture<real>>& g);
	~HenyeyGreenstein() override;

	void evalPhaseFunc(
		const SurfaceHit&     X,
		const math::Vector3R& I,
		const math::Vector3R& O,
		real*                 out_pf) const override;

private:
	std::shared_ptr<TTexture<real>> m_g;
};

}// end namespace ph
