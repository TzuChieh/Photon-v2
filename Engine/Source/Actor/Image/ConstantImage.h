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

	virtual void genTextureReal(
		CookingContext& context,
		std::shared_ptr<TTexture<real>>* out_texture) const override;

	virtual void genTextureVector3R(
		CookingContext& context,
		std::shared_ptr<TTexture<Vector3R>>* out_texture) const override;

	virtual void genTextureSpectral(
		CookingContext& context,
		std::shared_ptr<TTexture<SpectralStrength>>* out_texture) const override;

private:
	std::vector<real> m_values;

// command interface
public:
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
};

}// end namespace ph