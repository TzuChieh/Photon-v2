#pragma once

#include "Actor/Image/Image.h"
#include "Math/math_fwd.h"

#include <vector>

namespace ph
{

class ConstantImage final : public Image, public TCommandInterface<ConstantImage>
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
	explicit ConstantImage(const Vector3R& values);
	explicit ConstantImage(const std::vector<real>& values);
	ConstantImage(real value, EType type);
	ConstantImage(const Vector3R& values, EType type);
	ConstantImage(const std::vector<real>& values, EType type);

	std::shared_ptr<TTexture<real>> genTextureReal(
		CookingContext& context) const override;

	std::shared_ptr<TTexture<Vector3R>> genTextureVector3R(
		CookingContext& context) const override;

	std::shared_ptr<TTexture<SpectralStrength>> genTextureSpectral(
		CookingContext& context) const override;

private:
	std::vector<real> m_values;
	EType             m_type;

// command interface
public:
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
};

}// end namespace ph

/*
	<SDL_interface>

	<category>  image       </category>
	<type_name> constant    </type_name>
	<extend>    image.image </extend>

	<name> Constant Image </name>
	<description>
		An image that stores constant value. It can be a single real, a vector or a spectrum.
	</description>

	<command type="creator">
		<input name="value-type" type="string">
			<description>
				Specifying what the stored constant represents. "raw": the value will be used 
				directly without any conversion; "emr-linear-srgb": the value represents energy
				magnitudes in linear-SRGB; "ecf-linear-srgb": the value represents energy 
				conserving coefficients in linear-SRGB.
			</description>
		</input>
		<input name="value" type="real">
			<description>A single constant value.</description>
		</input>
		<input name="value" type="vector3">
			<description>Vectorized constant value with three elements.</description>
		</input>
	</command>

	</SDL_interface>
*/