#pragma once

#include "Core/Texture/TTexture.h"

#include <memory>

namespace ph
{

template<typename InputType, typename OutputType>
class TTextureFunction : public TTexture<OutputType>
{
public:
	inline TTextureFunction();
	inline TTextureFunction(const std::shared_ptr<TTexture<InputType>>& parentTexture);
	inline virtual ~TTextureFunction() override;

	inline virtual void sample(const SampleLocation& sampleLocation, OutputType* out_value) const = 0;

	inline void setParentTexture(const std::shared_ptr<TTexture<InputType>>& parentTexture);

private:
	std::shared_ptr<TTexture<InputType>> m_parentTexture;
};

}// end namespace ph

#include "Core/Texture/Function/TTextureFunction.ipp"