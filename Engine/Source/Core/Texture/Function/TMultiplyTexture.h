#pragma once

#include "Core/Texture/Function/TTextureFunction.h"

namespace ph
{

template
<
	typename InputType, 
	typename OutputType,
	//typename 
>
class TMultiplyTexture : public TTextureFunction<InputType, OutputType>
{
public:
	
};

}// end namespace ph

#include "Core/Texture/Function/TMultiplyTexture.ipp"