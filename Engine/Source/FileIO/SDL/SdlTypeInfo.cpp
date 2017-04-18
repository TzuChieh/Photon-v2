#include "FileIO/SDL/SdlTypeInfo.h"

namespace ph
{

SdlTypeInfo SdlTypeInfo::makeInvalid()
{
	return SdlTypeInfo(ETypeCategory::UNSPECIFIED, "");
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
	std::string categoryName;
	switch(typeCategory)
	{
	case ETypeCategory::UNSPECIFIED:      categoryName = "--unspecified--"; break;
	case ETypeCategory::PRIMITIVE:        categoryName = "primitive";       break;
	case ETypeCategory::REF_GEOMETRY:     categoryName = "geometry";        break;
	case ETypeCategory::REF_MATERIAL:     categoryName = "material";        break;
	case ETypeCategory::REF_LIGHT_SOURCE: categoryName = "light-source";    break;
	case ETypeCategory::REF_ACTOR:        categoryName = "actor";           break;
	}

	return categoryName;
}

std::string SdlTypeInfo::toString() const
{
	return "type_category <" + getCategoryName() + ">, type_name <" + typeName + ">";
}

}// end namespace ph