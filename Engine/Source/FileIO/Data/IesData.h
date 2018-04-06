#pragma once

#include "Common/primitive_type.h"
#include "FileIO/Data/IesFile.h"
#include "FileIO/FileSystem/Path.h"
#include "Common/Logger.h"

#include <vector>

namespace ph
{

/*
	This class processes the data from a given IES file and present it with
	a generalized interface.
*/
class IesData final
{
public:
	IesData(const Path& iesFilePath);

	real sampleAttenuationFactor(real theta, real phi) const;
	std::size_t numAttenuationFactorThetaSamples() const;
	std::size_t numAttenuationFactorPhiSamples() const;

private:
	IesFile                        m_file;

	// Candela values stored as data[phi-index][theta-index].
	// Range of theta: [0, pi]
	// Range of phi:   [0, 2 * pi]
	//
	std::vector<std::vector<real>> m_sphericalCandelas;

	std::vector<std::vector<real>> m_sphericalAttenuationFactors;

	enum class EReflectFrom
	{
		THETA_HEAD,
		THETA_TAIL,
		PHI_HEAD,
		PHI_TAIL
	};

	void processCandelaValues();
	void processAttenuationFactors();
	void reflectCandelaValues(EReflectFrom reflectFrom, real reflectionMultiplier = 1.0_r);

	static real calcBicubicWeight(real x);

	static const Logger logger;
};

}// end namespace ph