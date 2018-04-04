#include "FileIO/Data/IesData.h"
#include "Math/Math.h"

#include <cmath>
#include <algorithm>

namespace ph
{

const Logger IesData::logger(LogSender("IES Data"));

IesData::IesData(const Path& iesFilePath) : 
	m_file(IesFile(iesFilePath)),
	m_sphericalCandelas()
{
	processCandelaValues();
}

void IesData::processCandelaValues()
{
	if(m_file.getPhotometricWebType() != IesFile::EPhotometricWebType::C)
	{
		logger.log(ELogLevel::WARNING_MED, 
			"web type is not C, not supported (file: " + m_file.getFilename() + ")");
		return;
	}

	const auto vDegrees = m_file.getVerticalAngles();
	const auto hDegrees = m_file.getHorizontalAngles();
	if(!hDegrees.empty() && hDegrees[0] != 0)
	{
		logger.log(ELogLevel::WARNING_MED,
			"horizontal degrees does not start with 0, "
			"which is not supported (file: " + m_file.getFilename() + ")");
		return;
	}

	// Fill-in candela values and make it span the full spherical domain 
	// according to IES standard.
	// 
	// NOTE: The following implementations assume uniform angle interval.

	m_sphericalCandelas.clear();
	m_sphericalCandelas.resize(hDegrees.size(), std::vector<real>(vDegrees.size(), 0.0_r));

	const auto candelaValues = m_file.getCandelaValues();
	for(std::size_t hIndex = 0; hIndex < candelaValues.size(); hIndex++)
	{
		for(std::size_t vIndex = 0; vIndex < candelaValues[hIndex].size(); vIndex++)
		{
			PH_ASSERT(hIndex < m_sphericalCandelas.size() && 
			          vIndex < m_sphericalCandelas[hIndex].size());
			m_sphericalCandelas[hIndex][vIndex] = candelaValues[hIndex][vIndex];
		}
	}

	const real hDegreesDiff = hDegrees.back() - hDegrees.front();
	if(std::abs(hDegreesDiff - 0.0_r) < 0.0001_r)
	{
		// There is only one horizontal angle, implying that the luminaire
		// is laterally symmetric in all photometric planes. 
		// Reflect is still needed since phi is required to span [0, 2 * pi]
		//
		reflectCandelaValues(EReflectFrom::PHI_TAIL);
	}
	else if(std::abs(hDegreesDiff - 90.0_r) < 0.0001_r)
	{
		// The luminaire is assumed to be symmetric in each quadrant.
		//
		reflectCandelaValues(EReflectFrom::PHI_TAIL);
		reflectCandelaValues(EReflectFrom::PHI_TAIL);
	}
	else if(std::abs(hDegreesDiff - 180.0_r) < 0.0001_r)
	{
		// The luminaire is assumed to be bilaterally symmetric about the
		// 0 - 180 degree photometric plane.
		//
		reflectCandelaValues(EReflectFrom::PHI_TAIL);
	}
	else if(std::abs(hDegreesDiff - 360.0_r) < 0.0001_r)
	{
		// The luminaire is assumed to exhibit no lateral symmetry.
		//
		// Do nothing.
	}
	else
	{
		logger.log(ELogLevel::WARNING_MED,
			"unsupported angle difference (file: " + m_file.getFilename() + ")");
		return;
	}

	const real vDegreesDiff = vDegrees.back() - vDegrees.front();
	if(std::abs(vDegreesDiff - 90.0_r) < 0.0001_r)
	{
		if(std::abs(vDegrees.front() - 0.0_r) < 0.0001_r)
		{
			reflectCandelaValues(EReflectFrom::THETA_TAIL, 0.0_r);
		}
		else if(std::abs(vDegrees.front() - 90.0_r) < 0.0001_r)
		{
			reflectCandelaValues(EReflectFrom::THETA_HEAD, 0.0_r);
		}
	}
	else if(std::abs(vDegreesDiff - 180.0_r) < 0.0001_r)
	{
		// Already spans full range of theta.
		//
		// Do nothing.
	}
	else
	{
		logger.log(ELogLevel::WARNING_MED,
			"unsupported vertical angle difference (file: " + m_file.getFilename() + ")");
		return;
	}
}

void IesData::reflectCandelaValues(const EReflectFrom reflectFrom, const real reflectionMultiplier)
{
	if(reflectFrom == EReflectFrom::THETA_HEAD || reflectFrom == EReflectFrom::THETA_TAIL)
	{
		for(std::size_t pi = 0; pi < m_sphericalCandelas.size(); pi++)
		{
			std::vector<real> thetas = std::move(m_sphericalCandelas[pi]);

			std::vector<real> reflectedThetas = thetas;
			std::reverse(reflectedThetas.begin(), reflectedThetas.end());
			for(auto& value : reflectedThetas)
			{
				value *= reflectionMultiplier;
			}

			if(reflectFrom == EReflectFrom::THETA_HEAD)
			{
				reflectedThetas.insert(reflectedThetas.end() - 1, thetas.begin(), thetas.end());
				m_sphericalCandelas[pi] = std::move(reflectedThetas);
			}
			else
			{
				thetas.insert(thetas.end(), reflectedThetas.begin() + 1, reflectedThetas.end());
				m_sphericalCandelas[pi] = std::move(thetas);
			}
		}
	}
	else if(reflectFrom == EReflectFrom::PHI_HEAD || reflectFrom == EReflectFrom::PHI_TAIL)
	{
		std::vector<std::vector<real>> phis = std::move(m_sphericalCandelas);

		std::vector<std::vector<real>> reflectedPhis = phis;
		std::reverse(reflectedPhis.begin(), reflectedPhis.end());
		for(auto& values : reflectedPhis)
		{
			for(auto& value : values)
			{
				value *= reflectionMultiplier;
			}
		}

		if(reflectFrom == EReflectFrom::PHI_HEAD)
		{
			reflectedPhis.insert(reflectedPhis.end() - 1, phis.begin(), phis.end());
			m_sphericalCandelas = std::move(reflectedPhis);
		}
		else
		{
			phis.insert(phis.end(), reflectedPhis.begin() + 1, reflectedPhis.end());
			m_sphericalCandelas = std::move(phis);
		}
	}
}

}// end namespace ph