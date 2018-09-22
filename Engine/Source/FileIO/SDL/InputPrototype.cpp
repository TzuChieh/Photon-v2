#include "FileIO/SDL/InputPrototype.h"
#include "FileIO/SDL/Keyword.h"

namespace ph
{

void InputPrototype::addInteger(const std::string& name)
{
	typeNamePairs.push_back({Keyword::TYPENAME_INTEGER, name});
}

void InputPrototype::addReal(const std::string& name)
{
	typeNamePairs.push_back({Keyword::TYPENAME_REAL, name});
}

void InputPrototype::addString(const std::string& name)
{
	typeNamePairs.push_back({Keyword::TYPENAME_STRING, name});
}

void InputPrototype::addVector3r(const std::string& name)
{
	typeNamePairs.push_back({Keyword::TYPENAME_VECTOR3R, name});
}

void InputPrototype::addQuaternionR(const std::string& name)
{
	typeNamePairs.push_back({Keyword::TYPENAME_QUATERNIONR, name});
}

void InputPrototype::addRealArray(const std::string& name)
{
	typeNamePairs.push_back({Keyword::TYPENAME_REAL_ARRAY, name});
}

void InputPrototype::addVec3Array(const std::string& name)
{
	typeNamePairs.push_back({Keyword::TYPENAME_VECTOR3R_ARRAY, name});
}

std::string InputPrototype::toString() const
{
	std::string result;
	for(const auto& typeNamePair : typeNamePairs)
	{
		const std::string& typeString = std::string(typeNamePair.first);
		const std::string& nameString = typeNamePair.second;
		result += " [" + typeString + " " + nameString + " <value>] ";
	}
	return result;
}

}// end namespace ph