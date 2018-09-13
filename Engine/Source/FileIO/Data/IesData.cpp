#include "FileIO/Data/IesData.h"
#include "Math/math.h"
#include "Math/constant.h"

#include <cmath>
#include <algorithm>
#include <limits>

namespace ph
{

const Logger IesData::logger(LogSender("IES Data"));

IesData::IesData(const Path& iesFilePath) : 
	m_file(iesFilePath),
	m_sphericalCandelas(),
	m_sphericalAttenuationFactors()
{
	if(!m_file.load())
	{
		logger.log(ELogLevel::WARNING_MED, 
			"failed on loading file <" + m_file.getFilename() + ">");
		return;
	}

	processCandelaValues();
	processAttenuationFactors();
}

real IesData::sampleAttenuationFactor(const real theta, const real phi) const
{
	const int numThetaSamples = static_cast<int>(numAttenuationFactorThetaSamples());
	const int numPhiSamples   = static_cast<int>(numAttenuationFactorPhiSamples());
	if(numThetaSamples < 2 || numPhiSamples < 2)
	{
		return 0.0_r;
	}

	const real dTheta = PH_PI_REAL / static_cast<real>(numThetaSamples - 1);
	const real dPhi   = (2.0_r * PH_PI_REAL) / static_cast<real>(numPhiSamples - 1);

	const int minTi = static_cast<int>(std::ceil(theta / dTheta - 2.0_r));
	const int minPi = static_cast<int>(std::ceil(phi   / dPhi   - 2.0_r));
	const int maxTi = static_cast<int>(std::floor(theta / dTheta + 2.0_r));
	const int maxPi = static_cast<int>(std::floor(phi   / dPhi   + 2.0_r));

	real factor = 0.0_r, weight = 0.0_r;
	for(int pi = minPi; pi <= maxPi; pi++)
	{
		const real phi_i = pi * dPhi;
		const real wPhi  = calcBicubicWeight((phi_i - phi) / dPhi);
		for(int ti = minTi; ti <= maxTi; ti++)
		{
			const real theta_i = ti * dTheta;
			const real wTheta  = calcBicubicWeight((theta_i - theta) / dTheta);

			// clamping theta index and wrapping phi index
			//
			const int thetaIndex = math::clamp(ti, 0, numThetaSamples - 1);
			const int phiIndex   = math::wrap(pi, 0, numPhiSamples - 2);

			factor += wPhi * wTheta * m_sphericalAttenuationFactors[phiIndex][thetaIndex];
			weight += wPhi * wTheta;
		}
	}
	factor = weight != 0.0_r ? factor / weight : 0.0_r;

	// FIXME: Bicubic interpolation will cause overshoot in some places.
	// It is subtle but can be physically incorrect for attenuation factors.
	// Need a "smoother" solution here.
	return math::clamp(factor, 0.0_r, 1.0_r);
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
	// NOTE: The following implementations assume uniform angle interval,
	//       and does not handle multiplication factors specified in file.

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
		// Append a copy if 0-phi to make it span [0, 2 * pi]
		//
		auto zeroPhis = m_sphericalCandelas.front();
		m_sphericalCandelas.push_back(std::move(zeroPhis));
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

void IesData::processAttenuationFactors()
{
	// Assuming candela values has been processed.

	real maxCandela = std::numeric_limits<real>::min();
	real minCandela = std::numeric_limits<real>::max();
	for(const auto& candelas : m_sphericalCandelas)
	{
		for(const auto& candela : candelas)
		{
			if(candela > maxCandela)
			{
				maxCandela = candela;
			}
			if(candela < minCandela)
			{
				minCandela = candela;
			}
		}
	}

	if(minCandela < 0.0_r || maxCandela <= 0.0_r)
	{
		logger.log(ELogLevel::WARNING_MED, 
			"bad candela value detected in file <" + m_file.getFilename() + ">");
	}

	m_sphericalAttenuationFactors = m_sphericalCandelas;
	for(auto& factors : m_sphericalAttenuationFactors)
	{
		for(auto& factor : factors)
		{
			factor /= maxCandela;
		}
	}
}

void IesData::reflectCandelaValues(const EReflectFrom reflectFrom, const real reflectionMultiplier)
{
	if(reflectFrom == EReflectFrom::THETA_HEAD || reflectFrom == EReflectFrom::THETA_TAIL)
	{
		for(std::size_t pi = 0; pi < m_sphericalCandelas.size(); pi++)
		{
			std::vector<real> thetas = std::move(m_sphericalCandelas[pi]);
			PH_ASSERT(thetas.size() >= 2);

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
		PH_ASSERT(phis.size() >= 2);

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

std::size_t IesData::numAttenuationFactorThetaSamples() const
{
	if(!m_sphericalAttenuationFactors.empty())
	{
		return m_sphericalAttenuationFactors.front().size();
	}
	else
	{
		return 0;
	}
}

std::size_t IesData::numAttenuationFactorPhiSamples() const
{
	return m_sphericalAttenuationFactors.size();
}

// Reference: https://en.wikipedia.org/wiki/Bicubic_interpolation
//
real IesData::calcBicubicWeight(const real x)
{
	// corresponds to cubic Hermite spline
	//
	const real a = -0.5_r;

	const real absX = std::abs(x);
	if(absX <= 1)
	{
		return (a + 2) * absX * absX * absX - (a + 3) * absX * absX + 1;
	}
	else if(absX < 2)
	{
		return a * absX * absX * absX - 5 * a * absX * absX + 8 * a * absX - 4 * a;
	}
	else
	{
		return 0;
	}
}

}// end namespace ph