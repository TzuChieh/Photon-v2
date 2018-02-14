#pragma once

#include "Core/Texture/TAbstractPixelTex2D.h"
#include "Common/assertion.h"

#include <vector>
#include <memory>

namespace ph
{

template<typename T, std::size_t N>
class TMipmap : public TAbstractPixelTex2D<T, N>
{
public:
	inline TMipmap() : 
		TMipmap(1)
	{}

	explicit inline TMipmap(const std::size_t numLevels) : 
		TAbstractPixelTex2D<T, N>(),
		m_levels(numLevels, nullptr)
	{
		PH_ASSERT(numLevels >= 1);
	}

	virtual ~TMipmap() override = default;

	virtual void sample(
		const SampleLocation&  sampleLocation,
		TTexPixel<T, N>* const out_value) const = 0;

	inline const TAbstractPixelTex2D<T, N>* getLevel(const std::size_t level) const
	{
		PH_ASSERT(level < m_levels.size());

		return m_levels[level].get();
	}

	inline std::size_t numLevels() const
	{
		return m_levels.size();
	}

protected:
	inline void setLevel(const std::size_t level, 
	                     std::unique_ptr<TAbstractPixelTex2D<T, N>> texture)
	{
		PH_ASSERT(level < m_levels.size());

		m_levels[level] = std::move(texture);
	}

private:
	std::vector<std::unique_ptr<TAbstractPixelTex2D<T, N>>> m_levels;
};

}// end namespace ph