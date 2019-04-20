#pragma once

#include "Common/assertion.h"

#include <vector>
#include <cstddef>

namespace ph
{

template<typename EstimationType>
class TEstimationArray
{
public:
	TEstimationArray();
	explicit TEstimationArray(std::size_t numEstimations);

	std::size_t numEstimations() const;

	EstimationType& operator [] (std::size_t index);
	const EstimationType& operator [] (std::size_t index) const;

private:
	std::vector<EstimationType> m_estimations;
};

// In-header Implementations:

template<typename EstimationType>
inline TEstimationArray<EstimationType>::TEstimationArray() :
	TEstimationArray(0)
{}

template<typename EstimationType>
inline TEstimationArray<EstimationType>::TEstimationArray(const std::size_t numEstimations) :
	m_estimations(numEstimations, EstimationType(0))
{}

template<typename EstimationType>
inline std::size_t TEstimationArray<EstimationType>::numEstimations() const
{
	return m_estimations.size();
}

template<typename EstimationType>
inline EstimationType& TEstimationArray<EstimationType>::operator [] (const std::size_t index)
{
	PH_ASSERT_LT(index, m_estimations.size());

	return m_estimations[index];
}

template<typename EstimationType>
inline const EstimationType& TEstimationArray<EstimationType>::operator [] (const std::size_t index) const
{
	PH_ASSERT_LT(index, m_estimations.size());

	return m_estimations[index];
}

}// namespace ph