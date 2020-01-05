#include "DataIO/SDL/InputPrototype.h"
#include "DataIO/SDL/Keyword.h"

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

void InputPrototype::addVector3(const std::string& name)
{
	typeNamePairs.push_back({Keyword::TYPENAME_VECTOR3, name});
}

void InputPrototype::addQuaternion(const std::string& name)
{
	typeNamePairs.push_back({Keyword::TYPENAME_QUATERNION, name});
}

void InputPrototype::addRealArray(const std::string& name)
{
	typeNamePairs.push_back({Keyword::TYPENAME_REAL_ARRAY, name});
}

void InputPrototype::addVector3Array(const std::string& name)
{
	typeNamePairs.push_back({Keyword::TYPENAME_VECTOR3_ARRAY, name});
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
