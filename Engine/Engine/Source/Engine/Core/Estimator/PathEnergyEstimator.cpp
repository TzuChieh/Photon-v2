#include "Engine/Core/Estimator/PathEnergyEstimator.h"

#include <Common/logging.h>

namespace ph
{

PathEnergyEstimator::PathEnergyEstimator()
	: IRayEnergyEstimator()
	, m_estimationIdx(0)
{}

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
		PH_DEFAULT_LOG(Warning,
			"at PathEnergyEstimator::setEstimationIndex(), "
			"attempting to set estimation index for unsupported attribute; this estimator supports "
			"energy estimation only.");
	}
}

std::string PathEnergyEstimator::toString() const
{
	return "Path Energy Estimator";
}

}// end namespace ph