#pragma once

#include "Actor/Image/Image.h"
#include "Math/math_fwd.h"

#include <vector>

namespace ph
{

class ConstantImage final : public Image, public TCommandInterface<ConstantImage>
{
public:
	explicit ConstantImage(real value);
	explicit ConstantImage(const Vector3R& values);
	explicit ConstantImage(const std::vector<real>& values);
	virtual ~ConstantImage() override;

	virtual std::shared_ptr<TTexture<real>> genTextureReal(
		CookingContext& context) const override;

	virtual std::shared_ptr<TTexture<Vector3R>> genTextureVector3R(
		CookingContext& context) const override;

	virtual std::shared_ptr<TTexture<SpectralStrength>> genTextureSpectral(
		CookingContext& context) const override;

private:
	std::vector<real> m_values;

// command interface
public:
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
};

}// end namespace ph