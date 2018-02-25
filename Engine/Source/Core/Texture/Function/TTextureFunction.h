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
	inline TTextureFunction(const std::shared_ptr<TTexture<InputType>>& inputTexture);
	inline virtual ~TTextureFunction() override;

	inline virtual void sample(const SampleLocation& sampleLocation, OutputType* out_value) const = 0;

	inline TTextureFunction& setInputTexture(const std::shared_ptr<TTexture<InputType>>& parentTexture);

protected:
	inline const TTexture<InputType>* getInputTexture() const
	{
		return m_inputTexture.get();
	}

private:
	// FIXME: break the possibility of cyclic reference
	std::shared_ptr<TTexture<InputType>> m_inputTexture;
};

}// end namespace ph

#include "Core/Texture/Function/TTextureFunction.ipp"