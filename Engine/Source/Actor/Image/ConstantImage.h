#pragma once

#include "Actor/Image/Image.h"
#include "Math/math_fwd.h"

#include <vector>

namespace ph
{

class ConstantImage final : public Image
{
public:
	enum class EType
	{
		RAW,
		EMR_LINEAR_SRGB,
		ECF_LINEAR_SRGB,
		RAW_LINEAR_SRGB
	};

public:
	ConstantImage();
	explicit ConstantImage(real value);
	explicit ConstantImage(const math::Vector3R& values);
	explicit ConstantImage(const std::vector<real>& values);
	ConstantImage(real value, EType type);
	ConstantImage(const math::Vector3R& values, EType type);
	ConstantImage(const std::vector<real>& values, EType type);

	std::shared_ptr<TTexture<real>> genTextureReal(
		ActorCookingContext& ctx) const override;

	std::shared_ptr<TTexture<math::Vector3R>> genTextureVector3R(
		ActorCookingContext& ctx) const override;

	std::shared_ptr<TTexture<Spectrum>> genTextureSpectral(
		ActorCookingContext& ctx) const override;

private:
	std::vector<real> m_values;
	EType             m_type;
};

}// end namespace ph
