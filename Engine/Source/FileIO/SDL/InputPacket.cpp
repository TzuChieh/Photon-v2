#include "FileIO/SDL/InputPacket.h"
#include "FileIO/SDL/NamedResourceStorage.h"
#include "FileIO/SDL/ValueParser.h"
#include "FileIO/SDL/Keyword.h"
#include "FileIO/SDL/InputPrototype.h"
#include "FileIO/SDL/SdlParser.h"
#include "FileIO/SDL/SdlResourceIdentifier.h"

#include <iostream>

namespace ph
{

InputPacket::InputPacket(
	const std::vector<ValueClause>&   vClauses,
	const NamedResourceStorage* const storage,
	const Path&                       workingDirectory) :
	m_vClauses(vClauses), 
	m_storage(storage),
	m_workingDirectory(workingDirectory),
	m_valueParser(workingDirectory)
{}

InputPacket::InputPacket(InputPacket&& other) : 
	m_vClauses(std::move(other.m_vClauses)), 
	m_storage(std::move(other.m_storage)),
	m_workingDirectory(std::move(other.m_workingDirectory)),
	m_valueParser(std::move(other.m_valueParser))
{}

std::string InputPacket::getString(
	const std::string&   name, 
	const std::string&   defaultString, 
	const DataTreatment& treatment) const
{
	std::string stringValue;
	return findStringValue(Keyword::TYPENAME_STRING, name, treatment, &stringValue) ?
	       m_valueParser.parseString(stringValue) : defaultString;
}

integer InputPacket::getInteger(
	const std::string&   name, 
	const integer        defaultInteger, 
	const DataTreatment& treatment) const
{
	std::string stringValue;
	return findStringValue(Keyword::TYPENAME_INTEGER, name, treatment, &stringValue) ?
	       m_valueParser.parseInteger(stringValue) : defaultInteger;
}

real InputPacket::getReal(
	const std::string&   name, 
	const real           defaultReal, 
	const DataTreatment& treatment) const
{
	std::string stringValue;
	return findStringValue(Keyword::TYPENAME_REAL, name, treatment, &stringValue) ?
	       m_valueParser.parseReal(stringValue) : defaultReal;
}

Vector3R InputPacket::getVector3(
	const std::string&   name,
	const Vector3R&      defaultVector3, 
	const DataTreatment& treatment) const
{
	std::string stringValue;
	return findStringValue(Keyword::TYPENAME_VECTOR3, name, treatment, &stringValue) ?
	       m_valueParser.parseVector3(stringValue) : defaultVector3;
}

QuaternionR InputPacket::getQuaternion(
	const std::string&   name,
	const QuaternionR&   defaultQuaternion,
	const DataTreatment& treatment) const
{
	std::string stringValue;
	return findStringValue(Keyword::TYPENAME_QUATERNION, name, treatment, &stringValue) ?
	       m_valueParser.parseQuaternion(stringValue) : defaultQuaternion;
}

std::vector<real> InputPacket::getRealArray(
	const std::string&       name, 
	const std::vector<real>& defaultRealArray, 
	const DataTreatment&     treatment) const
{
	std::string stringValue;
	return findStringValue(Keyword::TYPENAME_REAL_ARRAY, name, treatment, &stringValue) ?
	       m_valueParser.parseRealArray(stringValue) : defaultRealArray;
}

std::vector<Vector3R> InputPacket::getVector3Array(
	const std::string&           name,
	const std::vector<Vector3R>& defaultVector3Array,
	const DataTreatment&         treatment) const
{
	std::string stringValue;
	return findStringValue(Keyword::TYPENAME_VECTOR3_ARRAY, name, treatment, &stringValue) ?
	       m_valueParser.parseVector3Array(stringValue) : defaultVector3Array;
}

Path InputPacket::getStringAsPath(
	const std::string&   name,
	const Path&          defaultPath,
	const DataTreatment& treatment) const
{
	const std::string& identifierString = getString(name, defaultPath.toString(), treatment);
	return SdlResourceIdentifier(identifierString, m_workingDirectory).getPathToResource();
}

bool InputPacket::hasString(const std::string& name) const
{
	return findStringValue(Keyword::TYPENAME_STRING, name, DataTreatment(), nullptr);
}

bool InputPacket::hasInteger(const std::string& name) const
{
	return findStringValue(Keyword::TYPENAME_INTEGER, name, DataTreatment(), nullptr);
}

bool InputPacket::hasReal(const std::string& name) const
{
	return findStringValue(Keyword::TYPENAME_REAL, name, DataTreatment(), nullptr);
}

bool InputPacket::hasVector3(const std::string& name) const
{
	return findStringValue(Keyword::TYPENAME_VECTOR3, name, DataTreatment(), nullptr);
}

bool InputPacket::hasQuaternion(const std::string& name) const
{
	return findStringValue(Keyword::TYPENAME_QUATERNION, name, DataTreatment(), nullptr);
}

bool InputPacket::isPrototypeMatched(const InputPrototype& prototype) const
{
	for(const auto& typeNamePair : prototype.typeNamePairs)
	{
		const std::string_view& typeString = typeNamePair.first;
		const std::string&      nameString = typeNamePair.second;
		if(!findStringValue(typeString, nameString, DataTreatment(), nullptr))
		{
			return false;
		}
	}

	return true;
}

bool InputPacket::findStringValue(
	const std::string_view typeName, const std::string& dataName, const DataTreatment& treatment,
	std::string* const out_value) const
{
	if(out_value)
	{
		out_value->clear();
	}

	for(const ValueClause& vClause : m_vClauses)
	{
		if(vClause.type == typeName && vClause.name == dataName)
		{
			if(out_value)
			{
				*out_value = vClause.value;
			}

			return true;
		}
	}

	reportDataNotFound(typeName, dataName, treatment);
	return false;
}

void InputPacket::reportDataNotFound(const std::string_view typeName, const std::string& dataName, const DataTreatment& treatment)
{
	const std::string& message = treatment.notFoundInfo;

	switch(treatment.importance)
	{
	case EDataImportance::OPTIONAL:
		if(!message.empty())
		{
			std::cerr << "warning: optional data type<" << typeName << "> name<" << dataName << "> not found (" << message << ")" << std::endl;
		}
		break;

	case EDataImportance::REQUIRED:
		std::cerr << "warning: required data type<" << typeName << "> name<" << dataName << "> not found";
		if(!message.empty())
		{
			std::cerr << " (" << message << ")";
		}
		std::cerr << std::endl;
		break;
	}
}

std::string InputPacket::getCoreDataName()
{
	return SdlParser::CORE_DATA_NAME();
}

Path InputPacket::sdlResourceIdentifierToPath(const std::string& sdlResourceIdentifier) const
{
	return m_workingDirectory.append(Path(sdlResourceIdentifier));
}

}// end namespace ph