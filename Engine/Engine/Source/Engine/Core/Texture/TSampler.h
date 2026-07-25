#pragma once

#include "Engine/Core/Texture/TTexture.h"
#include "Engine/Core/SurfaceHit.h"
#include "Engine/Core/Texture/SampleLocation.h"
#include "Engine/Math/math_fwd.h"
#include "Engine/Math/Color/color_enums.h"

#include <Common/primitive_type.h>

namespace ph
{

/*! @brief Common settings and operations for sampling a texture.
*/
template<typename OutputType>
class TSampler final
{
public:
	TSampler() :
		TSampler(math::EColorUsage::Raw)
	{}

	TSampler(const math::EColorUsage sampleUsage) :
		TSampler(sampleUsage, 0)
	{}

	TSampler(const math::EColorUsage sampleUsage, const uint32 sampledChannel) :
		m_sampleUsage(sampleUsage), m_sampledChannel(sampledChannel)
	{}

	OutputType sample(const TTexture<OutputType>& texture, const SurfaceHit& X) const
	{
		OutputType value;
		if(m_sampledChannel == X.getProbe().getChannel())
		{
			texture.sample(SampleLocation(&X, m_sampleUsage), &value);
		}
		else
		{
			const SurfaceHit sampledX = X.switchChannel(m_sampledChannel);
			texture.sample(SampleLocation(&sampledX, m_sampleUsage), &value);
		}
		return value;
	}

	OutputType sample(const TTexture<OutputType>& texture, const math::Vector3R& uvw) const
	{
		OutputType value;
		texture.sample(SampleLocation(uvw, m_sampleUsage), &value);
		return value;
	}

	OutputType sample(const TTexture<OutputType>& texture, const math::Vector2R& uv) const
	{
		OutputType value;
		texture.sample(SampleLocation(uv, m_sampleUsage), &value);
		return value;
	}

	OutputType sampleOrDefault(
		const TTexture<OutputType>* texture,
		const SurfaceHit& X,
		const OutputType& defaultValue) const
	{
		if(texture)
		{
			return sample(*texture, X);
		}
		else
		{
			return defaultValue;
		}
	}

private:
	math::EColorUsage m_sampleUsage;
	uint32            m_sampledChannel;
};

}// end namespace ph
