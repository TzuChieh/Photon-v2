#pragma once

#include "Core/Texture/TTexture.h"

#include <type_traits>
#include <memory>

namespace ph
{

template<typename InputType, typename OutputType, typename Function>
class TTextureFunction : public TTexture<OutputType>
{
	static_assert(
		std::is_same_v<
			decltype(std::declval<Function>()(std::declval<InputType>())),
			OutputType>,
		"<Function> must be callable as <OutputType(InputType)>");

public:
	using InputTextureResource = std::shared_ptr<TTexture<InputType>>;

	template<typename = std::enable_if_t<std::is_default_constructible_v<Function>>>
	explicit TTextureFunction(InputTextureResource inputTexture);

	TTextureFunction(InputTextureResource inputTexture, Function function);

	void sample(const SampleLocation& sampleLocation, OutputType* out_value) const override;

private:
	InputTextureResource m_inputTexture;
	Function             m_function;
};

}// end namespace ph

#include "Core/Texture/Function/TTextureFunction.ipp"
