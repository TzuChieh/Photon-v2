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

private:
	IesFile                        m_file;

	// Candela values stored as data[phi-index][theta-index].
	// Range of theta: [0, pi]
	// Range of phi:   [0, 2 * pi]
	//
	std::vector<std::vector<real>> m_sphericalCandelas;

	enum class EReflectFrom
	{
		THETA_HEAD,
		THETA_TAIL,
		PHI_HEAD,
		PHI_TAIL
	};

	void processCandelaValues();
	void reflectCandelaValues(EReflectFrom reflectFrom, real reflectionMultiplier = 1.0_r);

	static const Logger logger;
};

}// end namespace ph