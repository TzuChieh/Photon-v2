#include "DataIO/SDL/SdlTypeInfo.h"

#include <unordered_map>
#include <iostream>

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
	case ETypeCategory::REF_GEOMETRY:         categoryName = "geometry";         break;
	case ETypeCategory::REF_MOTION:           categoryName = "motion";           break;
	case ETypeCategory::REF_MATERIAL:         categoryName = "material";         break;
	case ETypeCategory::REF_LIGHT_SOURCE:     categoryName = "light-source";     break;
	case ETypeCategory::REF_ACTOR:            categoryName = "actor";            break;
	case ETypeCategory::REF_FRAME_PROCESSOR:  categoryName = "frame-processor";  break;
	case ETypeCategory::REF_IMAGE:            categoryName = "image";            break;

	case ETypeCategory::REF_RECEIVER:         categoryName = "receiver";         break;
	case ETypeCategory::REF_SAMPLE_GENERATOR: categoryName = "sample-generator"; break;
	case ETypeCategory::REF_RENDERER:         categoryName = "renderer";         break;
	case ETypeCategory::REF_OPTION:           categoryName = "option";           break;

	default:
		std::cerr << "warning: at SdlTypeInfo::categoryToName() " 
		          << "unspecified category detected, "
		          << "converting category to name failed" << std::endl;
		break;
	}

	return categoryName;
}

ETypeCategory SdlTypeInfo::nameToCategory(const std::string& name)
{
	const static std::unordered_map<std::string, ETypeCategory> map = 
	{ 
		{categoryToName(ETypeCategory::REF_GEOMETRY),         ETypeCategory::REF_GEOMETRY},
		{categoryToName(ETypeCategory::REF_MATERIAL),         ETypeCategory::REF_MATERIAL},
		{categoryToName(ETypeCategory::REF_MOTION),           ETypeCategory::REF_MOTION},
		{categoryToName(ETypeCategory::REF_LIGHT_SOURCE),     ETypeCategory::REF_LIGHT_SOURCE},
		{categoryToName(ETypeCategory::REF_ACTOR),            ETypeCategory::REF_ACTOR},
		{categoryToName(ETypeCategory::REF_RECEIVER),         ETypeCategory::REF_RECEIVER},
		{categoryToName(ETypeCategory::REF_IMAGE),            ETypeCategory::REF_IMAGE},

		{categoryToName(ETypeCategory::REF_SAMPLE_GENERATOR), ETypeCategory::REF_SAMPLE_GENERATOR},
		{categoryToName(ETypeCategory::REF_FRAME_PROCESSOR),  ETypeCategory::REF_FRAME_PROCESSOR},
		{categoryToName(ETypeCategory::REF_RENDERER),         ETypeCategory::REF_RENDERER},
		{categoryToName(ETypeCategory::REF_OPTION),           ETypeCategory::REF_OPTION}
	};

	const auto& iter = map.find(name);
	if(iter == map.end())
	{
		std::cerr << "warning: at SdlTypeInfo::nameToCategory(), "
		          << "converting name <" << name << "> to category failed, "
		          << "returning unspecified category" << std::endl;
		return ETypeCategory::UNSPECIFIED;
	}

	return iter->second;
}

SdlTypeInfo::SdlTypeInfo(const ETypeCategory typeCategory, const std::string& typeName) :
	typeCategory(typeCategory), typeName(typeName)
{}

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
