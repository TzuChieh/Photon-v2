#pragma once

#include "FileIO/SDL/ETypeCategory.h"

#include <string>

namespace ph
{

class SdlTypeInfo final
{
public:
	static SdlTypeInfo makeInvalid();
	static std::string categoryToName(const ETypeCategory typeCategory);
	static ETypeCategory nameToCategory(const std::string& name);

public:
	ETypeCategory typeCategory;
	std::string   typeName;

	SdlTypeInfo(const ETypeCategory typeCategory, const std::string& typeName);

	bool isValid() const;
	std::string getCategoryName() const;
	std::string toString() const;
};

}// end namespace ph