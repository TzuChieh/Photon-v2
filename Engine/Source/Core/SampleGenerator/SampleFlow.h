#pragma once

#include "Common/primitive_type.h"
#include "Common/assertion.h"
#include "Math/Random.h"
#include "Math/Random/sample.h"

#include <cstddef>
#include <array>
#include <optional>

namespace ph
{

// TODO: test
class SampleFlow final
{
public:
	SampleFlow();
	SampleFlow(const real* savedDims, std::size_t numSavedDims);

	real flow1D();
	std::array<real, 2> flow2D();
	std::array<real, 3> flow3D();

	template<std::size_t N>
	std::array<real, N> flowND();

	bool pick(real pickProbability);
	bool unflowedPick(real pickProbability);

	// TODO: non-const is intentional for future sample recording
	bool randomPick(real pickProbability);

private:
	const real*         m_savedDims;
	std::size_t         m_numSavedDims;
	std::size_t         m_numReadDims;
	std::optional<real> m_partiallyUsedDim;

	bool hasMoreToRead() const;
	real load1D();
};

// In-header Implementations:

inline SampleFlow::SampleFlow() :
	m_savedDims       (nullptr),
	m_numSavedDims    (0),
	m_numReadDims     (0),
	m_partiallyUsedDim()
{}

inline SampleFlow::SampleFlow(const real* const savedDims, const std::size_t numSavedDims) :
	m_savedDims       (savedDims),
	m_numSavedDims    (numSavedDims),
	m_numReadDims     (0),
	m_partiallyUsedDim()
{
	PH_ASSERT(savedDims);
}

inline real SampleFlow::flow1D()
{
	return flowND<1>()[0];
}

inline std::array<real, 2> SampleFlow::flow2D()
{
	return flowND<2>();
}

inline std::array<real, 3> SampleFlow::flow3D()
{
	return flowND<3>();
}

template<std::size_t N>
inline std::array<real, N> SampleFlow::flowND()
{
	std::array<real, N> sample;
	for(std::size_t i = 0; i < N; ++i)
	{
		sample[i] = load1D();
	}
	return sample;
}

inline bool SampleFlow::pick(const real pickProbability)
{
	return math::pick(pickProbability, load1D());
}

inline bool SampleFlow::unflowedPick(const real pickProbability)
{
	real dimValue = load1D();
	const bool isPicked = math::reused_pick(pickProbability, dimValue);

	PH_ASSERT(!m_partiallyUsedDim.has_value());
	m_partiallyUsedDim = dimValue;

	return isPicked;
}

inline bool SampleFlow::randomPick(const real pickProbability)
{
	return math::pick(pickProbability, math::Random::genUniformReal_i0_e1());
}

inline bool SampleFlow::hasMoreToRead() const
{
	return m_numReadDims < m_numSavedDims;
}

inline real SampleFlow::load1D()
{
	if(m_partiallyUsedDim.has_value())
	{
		const real dimValue = m_partiallyUsedDim.value();
		m_partiallyUsedDim.reset();
		return dimValue;
	}

	return m_savedDims && hasMoreToRead() ?
		m_savedDims[m_numReadDims++] : math::Random::genUniformReal_i0_e1();
}

}// end namespace ph
