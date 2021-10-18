#pragma once

#include "Core/Texture/TTexture.h"

#include <type_traits>
#include <memory>

namespace ph
{

template<typename InputType, typename OutputType, typename OperatorType>
class TTextureOperator : public TTexture<OutputType>
{
	static_assert(
		std::is_same_v<
			decltype(std::declval<OperatorType>()(std::declval<InputType>())),
			OutputType>,
		"<OperatorType> must be callable as <OutputType(InputType)>");

public:
	using InputTextureResource = std::shared_ptr<TTexture<InputType>>;

	template<typename = std::enable_if_t<std::is_default_constructible_v<OperatorType>>>
	explicit TTextureOperator(InputTextureResource inputTexture);

	TTextureOperator(InputTextureResource inputTexture, OperatorType op);

	void sample(const SampleLocation& sampleLocation, OutputType* out_value) const override;

private:
	InputTextureResource m_inputTexture;
	OperatorType         m_operator;
};

}// end namespace ph
