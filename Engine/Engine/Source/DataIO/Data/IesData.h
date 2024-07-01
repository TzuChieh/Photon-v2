#pragma once

#include "DataIO/Data/IesFile.h"
#include "DataIO/FileSystem/Path.h"

#include <Common/primitive_type.h>

#include <vector>

namespace ph
{

/*!
This class processes the data from a given IES file and present it with
a generalized interface.
*/
class IesData
{
public:
	explicit IesData(const Path& iesFilePath);

	real sampleAttenuationFactor(real theta, real phi) const;
	std::size_t numAttenuationFactorThetaSamples() const;
	std::size_t numAttenuationFactorPhiSamples() const;

private:
	IesFile                        m_file;

	/*!
	Candela values stored as `data[phi_index][theta_index]`.
	Range of theta: [0, pi]
	Range of phi:   [0, 2 * pi]
	*/
	std::vector<std::vector<real>> m_sphericalCandelas;

	std::vector<std::vector<real>> m_sphericalAttenuationFactors;

	enum class EReflectFrom
	{
		Theta_Head,
		Theta_Tail,
		Phi_Head,
		Phi_Tail
	};

	void processCandelaValues();
	void processAttenuationFactors();
	void reflectCandelaValues(EReflectFrom reflectFrom, real reflectionMultiplier = 1.0_r);

	static real calcBicubicWeight(real x);
};

}// end namespace ph
