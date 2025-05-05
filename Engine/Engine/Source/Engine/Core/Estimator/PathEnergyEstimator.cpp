#include "Engine/Core/Estimator/PathEnergyEstimator.h"

namespace ph
{

PathEnergyEstimator::PathEnergyEstimator()

	: IRayEnergyEstimator()

	, m_params()
	, m_estimationIdx(0)
{}

void PathEnergyEstimator::setPTParams(PTEstimatorParams params)
{
	m_params = params;
}

void PathEnergyEstimator::setEstimationIndex(
	const EEstimatorAttribute attribute,
	const int estimationIdx)
{
	if(attribute == EEstimatorAttribute::Energy)
	{
		m_estimationIdx = estimationIdx;
	}
	else
	{
		setEstimationIndex(attribute, estimationIdx);
	}
}

std::string PathEnergyEstimator::toString() const
{
	return "Path Energy Estimator";
}

}// end namespace ph