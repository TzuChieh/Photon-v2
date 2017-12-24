#pragma once

#include "Core/Texture/TTexture.h"

namespace ph
{

template<typename OutputType>
class TConstantTexture final : public TTexture<OutputType>
{
public:
	inline TConstantTexture(const OutputType& value);
	inline virtual ~TConstantTexture() override;

	inline virtual void sample(const Vector3R& uvw, OutputType* out_value) const override;

private:
	OutputType m_value;
};

}// end namespace ph

#include "Core/Texture/TConstantTexture.ipp"