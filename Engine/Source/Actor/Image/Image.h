#pragma once

#include "Common/primitive_type.h"
#include "Math/TVector3.h"
#include "FileIO/SDL/TCommandInterface.h"
#include "Core/Texture/TTexture.h"
#include "Core/Quantity/SpectralStrength.h"
#include "Core/Texture/TConstantTexture.h"

#include <iostream>
#include <memory>

namespace ph
{

class CookingContext;

class Image : public TCommandInterface<Image>
{
public:
	virtual ~Image() = 0;

	virtual void genTexture(CookingContext& context, 
	                        std::shared_ptr<TTexture<real>>* out_texture) const;
	virtual void genTexture(CookingContext& context,
	                        std::shared_ptr<TTexture<Vector3R>>* out_texture) const;
	virtual void genTexture(CookingContext& context,
	                        std::shared_ptr<TTexture<SpectralStrength>>* out_texture) const;

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
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
};

}// end namespace ph