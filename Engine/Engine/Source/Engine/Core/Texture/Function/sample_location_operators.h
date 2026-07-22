#pragma once

#include "Engine/Core/Texture/SampleLocation.h"
#include "Engine/Core/Texture/TTexture.h"
#include "Engine/Math/TMatrix4.h"
#include "Engine/Math/TVector3.h"

#include <Common/assertion.h>

#include <concepts>
#include <memory>
#include <utility>

namespace ph
{

namespace texfunc
{

template<typename OperatorType>
concept CSampleLocationOperator = requires (OperatorType op, SampleLocation sampleLocation)
{
	{ op(sampleLocation) } -> std::same_as<SampleLocation>;
};

/*! @brief Applies an affine transformation to UVW coordinates.

All other sample location data is preserved.
*/
class AffineUvwTransform final
{
public:
	explicit AffineUvwTransform(const math::Matrix4R& transform)
		: m_transform(transform)
	{}

	SampleLocation operator () (const SampleLocation& sampleLocation) const
	{
		math::Vector3R transformedUvw;
		m_transform.mul(sampleLocation.uvw(), 1.0_r, &transformedUvw);

		SampleLocation transformedLocation(sampleLocation);
		transformedLocation.setUvw(transformedUvw);
		return transformedLocation;
	}

private:
	math::Matrix4R m_transform;
};

}// end namespace texfunc

/*! @brief Maps the sample location before sampling an input texture.

`OperatorType` receives the original sample location and returns the location used to sample the
wrapped texture.
*/
template
<
	typename OutputType,
	texfunc::CSampleLocationOperator OperatorType
>
class TSampleLocationOperator : public TTexture<OutputType>
{
public:
	using InputTexRes = std::shared_ptr<TTexture<OutputType>>;

	explicit TSampleLocationOperator(InputTexRes inputTexture)
		requires std::default_initializable<OperatorType>
		: TSampleLocationOperator(std::move(inputTexture), OperatorType{})
	{}

	TSampleLocationOperator(InputTexRes inputTexture, OperatorType op)
		: m_inputTexture(std::move(inputTexture))
		, m_operator(std::move(op))
	{}

	void sample(const SampleLocation& sampleLocation, OutputType* const out_value) const override
	{
		PH_ASSERT(m_inputTexture);
		PH_ASSERT(out_value);

		m_inputTexture->sample(m_operator(sampleLocation), out_value);
	}

private:
	InputTexRes m_inputTexture;
	OperatorType m_operator;
};

}// end namespace ph
