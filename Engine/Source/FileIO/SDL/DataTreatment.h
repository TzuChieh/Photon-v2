#pragma once

#include <string>

namespace ph
{

enum class EDataImportance
{
	OPTIONAL,
	REQUIRED
};

class DataTreatment final
{
public:
	static inline DataTreatment OPTIONAL(const std::string& notFoundInfo = "")
	{
		return DataTreatment(EDataImportance::OPTIONAL, notFoundInfo);
	}

	static inline DataTreatment REQUIRED(const std::string& notFoundInfo = "")
	{
		return DataTreatment(EDataImportance::REQUIRED, notFoundInfo);
	}

	EDataImportance importance;
	std::string     notFoundInfo;

	inline DataTreatment(const EDataImportance importance = EDataImportance::OPTIONAL,
	                     const std::string& notFoundInfo = "") : 
		importance(importance), notFoundInfo(notFoundInfo)
	{

	}
};

}// end namespace ph