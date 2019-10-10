#pragma once

#include "Core/Texture/TTexture.h"
#include "Core/SurfaceHit.h"
#include "Core/Texture/SampleLocation.h"
#include "Common/primitive_type.h"
#include "Math/math_fwd.h"

namespace ph
{

template<typename OutputType>
class TSampler final
{
public:
	inline TSampler() :
		TSampler(EQuantity::RAW)
	{}

	inline TSampler(const EQuantity sampledQuantity) : 
		TSampler(sampledQuantity, 0)
	{}

	inline TSampler(const EQuantity sampledQuantity, const uint32 sampledChannel) :
		m_sampledQuantity(sampledQuantity), m_sampledChannel(sampledChannel)
	{}

	inline OutputType sample(const TTexture<OutputType>& texture, const SurfaceHit& X) const
	{
		HitDetail channeledDetail = X.getDetail();
		if(m_sampledChannel != 0)
		{
			channeledDetail = X.switchChannel(m_sampledChannel).getDetail();
		}

		OutputType value;
		texture.sample(SampleLocation(channeledDetail, m_sampledQuantity), &value);
		return value;
	}

	inline OutputType sample(const TTexture<OutputType>& texture, const math::Vector3R& uvw) const
	{
		OutputType value;
		texture.sample(SampleLocation(uvw, m_sampledQuantity), &value);
		return value;
	}

	inline OutputType sample(const TTexture<OutputType>& texture, const math::Vector2R& uv) const
	{
		OutputType value;
		texture.sample(SampleLocation(uv, m_sampledQuantity), &value);
		return value;
	}

private:
	EQuantity m_sampledQuantity;
	uint32    m_sampledChannel;
};

}// end namespace ph
