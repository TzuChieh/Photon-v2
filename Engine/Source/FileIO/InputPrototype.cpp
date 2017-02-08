#include "FileIO/InputPrototype.h"
#include "FileIO/Keyword.h"

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

std::string InputPrototype::toString() const
{
	std::string result;
	for(const auto& typeNamePair : typeNamePairs)
	{
		const std::string& typeString = typeNamePair.first;
		const std::string& nameString = typeNamePair.second;
		result += " [" + typeString + " " + nameString + " <value>] ";
	}
	return result;
}

}// end namespace ph