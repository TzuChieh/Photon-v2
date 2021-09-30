#pragma once

#include "Core/Texture/TTexture.h"
#include "Core/SurfaceHit.h"
#include "Core/Texture/SampleLocation.h"
#include "Common/primitive_type.h"
#include "Math/math_fwd.h"
#include "Math/Color/color_enums.h"

namespace ph
{

template<typename OutputType>
class TSampler final
{
public:
	inline TSampler() :
		TSampler(math::EColorUsage::RAW)
	{}

	inline TSampler(const math::EColorUsage sampleUsage) :
		TSampler(sampleUsage, 0)
	{}

	inline TSampler(const math::EColorUsage sampleUsage, const uint32 sampledChannel) :
		m_sampleUsage(sampleUsage), m_sampledChannel(sampledChannel)
	{}

	inline OutputType sample(const TTexture<OutputType>& texture, const SurfaceHit& X) const
	{
		HitDetail channeledDetail = X.getDetail();
		if(m_sampledChannel != 0)
		{
			channeledDetail = X.switchChannel(m_sampledChannel).getDetail();
		}

		OutputType value;
		texture.sample(SampleLocation(channeledDetail, m_sampleUsage), &value);
		return value;
	}

	inline OutputType sample(const TTexture<OutputType>& texture, const math::Vector3R& uvw) const
	{
		OutputType value;
		texture.sample(SampleLocation(uvw, m_sampleUsage), &value);
		return value;
	}

	inline OutputType sample(const TTexture<OutputType>& texture, const math::Vector2R& uv) const
	{
		OutputType value;
		texture.sample(SampleLocation(uv, m_sampleUsage), &value);
		return value;
	}

private:
	math::EColorUsage m_sampleUsage;
	uint32            m_sampledChannel;
};

}// end namespace ph
