#pragma once

#include "Common/primitive_type.h"
#include "Math/TVector3.h"

namespace ph
{

class RadianceSensor final
{
public:
	float64 m_accuR;
	float64 m_accuG;
	float64 m_accuB;
	uint32  m_numSenseCounts;

	RadianceSensor() : 
		m_accuR(0), m_accuG(0), m_accuB(0), m_numSenseCounts(0)
	{

	}
};

}// end namespace ph