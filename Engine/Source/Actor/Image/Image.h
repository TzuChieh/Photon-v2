#pragma once

#include "Common/primitive_type.h"
#include "Math/TVector3.h"
#include "DataIO/SDL/TCommandInterface.h"
#include "Core/Texture/TTexture.h"
#include "Core/Quantity/Spectrum.h"
#include "Core/Texture/TConstantTexture.h"

#include <iostream>
#include <memory>

namespace ph
{

class CookingContext;
class InputPacket;

class Image : public TCommandInterface<Image>
{
public:
	Image();
	virtual ~Image() = default;

	virtual std::shared_ptr<TTexture<real>> genTextureReal(
		CookingContext& context) const;

	virtual std::shared_ptr<TTexture<math::Vector3R>> genTextureVector3R(
		CookingContext& context) const;

	virtual std::shared_ptr<TTexture<Spectrum>> genTextureSpectral(
		CookingContext& context) const;

private:
	template<typename OutputType>
	inline std::shared_ptr<TTexture<OutputType>> genDefaultTexture() const
	{
		std::cerr << "warning: at Image::genTexture(), "
	              << "no implementation provided, generating a constant one" << std::endl;

		return std::make_shared<TConstantTexture<OutputType>>(OutputType(1));
	}

// command interface
public:
	explicit Image(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
	static void registerMathFunctions(CommandRegister& cmdRegister);
};

}// end namespace ph

#include "Actor/Image/Image.ipp"

/*
	<SDL_interface>

	<category>  image </category>
	<type_name> image </type_name>

	<name> Image </name>
	<description>
		A block of data.
	</description>

	</SDL_interface>
*/
