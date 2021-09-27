#pragma once

namespace ph
{

class SampleLocation;

template<typename OutputType>
class TTexture
{
public:
	using Output = OutputType;

public:
	inline TTexture() = default;
	inline virtual ~TTexture() = default;

	virtual void sample(const SampleLocation& sampleLocation, Output* out_value) const = 0;
};

}// end namespace ph
