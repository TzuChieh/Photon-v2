#include "FileIO/SDL/SdlTypeInfo.h"

#include <unordered_map>
#include <iostream>

#define SDL_TYPE_INFO_UNSPECIFIED

namespace ph
{

SdlTypeInfo SdlTypeInfo::makeInvalid()
{
	return SdlTypeInfo(ETypeCategory::UNSPECIFIED, "");
}

std::string SdlTypeInfo::categoryToName(const ETypeCategory typeCategory)
{
	std::string categoryName;
	switch(typeCategory)
	{
	case ETypeCategory::PRIMITIVE:        categoryName = "primitive";       break;
	case ETypeCategory::REF_GEOMETRY:     categoryName = "geometry";        break;
	case ETypeCategory::REF_MATERIAL:     categoryName = "material";        break;
	case ETypeCategory::REF_LIGHT_SOURCE: categoryName = "light-source";    break;
	case ETypeCategory::REF_ACTOR:        categoryName = "actor";           break;

	default:
		std::cerr << "warning: converting category to name failed, unspecified category detected" << std::endl;
		break;
	}

	return categoryName;
}

ETypeCategory SdlTypeInfo::nameToCategory(const std::string& name)
{
	const static std::unordered_map<std::string, ETypeCategory> map = 
	{ 
		{categoryToName(ETypeCategory::PRIMITIVE),        ETypeCategory::PRIMITIVE},
		{categoryToName(ETypeCategory::REF_GEOMETRY),     ETypeCategory::REF_GEOMETRY},
		{categoryToName(ETypeCategory::REF_MATERIAL),     ETypeCategory::REF_MATERIAL},
		{categoryToName(ETypeCategory::REF_LIGHT_SOURCE), ETypeCategory::REF_LIGHT_SOURCE},
		{categoryToName(ETypeCategory::REF_ACTOR),        ETypeCategory::REF_ACTOR},
	};

	const auto& iter = map.find(name);
	if(iter == map.end())
	{
		std::cerr << "warning: converting name to category failed, returning unspecified category" << std::endl;
		return ETypeCategory::UNSPECIFIED;
	}

	return iter->second;
}

SdlTypeInfo::SdlTypeInfo(const ETypeCategory typeCategory, const std::string& typeName) :
	typeCategory(typeCategory), typeName(typeName)
{

}

bool SdlTypeInfo::isValid() const
{
	return typeCategory != ETypeCategory::UNSPECIFIED && !typeName.empty();
}

std::string SdlTypeInfo::getCategoryName() const
{
	return categoryToName(typeCategory);
}

std::string SdlTypeInfo::toString() const
{
	return "type_category <" + getCategoryName() + ">, type_name <" + typeName + ">";
}

}// end namespace ph