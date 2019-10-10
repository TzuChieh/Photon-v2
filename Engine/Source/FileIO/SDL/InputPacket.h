#pragma once

#include "Common/primitive_type.h"
#include "FileIO/SDL/ValueClause.h"
#include "Math/TVector3.h"
#include "Math/TQuaternion.h"
#include "FileIO/SDL/DataTreatment.h"
#include "FileIO/SDL/SdlTypeInfo.h"
#include "FileIO/SDL/NamedResourceStorage.h"
#include "FileIO/FileSystem/Path.h"
#include "FileIO/SDL/ValueParser.h"

#include <vector>
#include <string>
#include <memory>
#include <string_view>

namespace ph
{

class NamedResourceStorage;
class Geometry;
class Texture;
class Material;
class LightSource;
class PhysicalActor;
class InputPrototype;

class InputPacket final
{
public:
	InputPacket(
		const std::vector<ValueClause>& vClauses, 
		const NamedResourceStorage*     storage,
		const Path&                     workingDirectory);

	InputPacket(InputPacket&& other);
	
	auto getString(
		const std::string&   name, 
		const std::string&   defaultValue = "",
		const DataTreatment& treatment    = DataTreatment()) const
	-> std::string;

	auto getInteger(
		const std::string&   name,
		const integer        defaultValue = 0,
		const DataTreatment& treatment    = DataTreatment()) const
	-> integer;

	auto getReal(
		const std::string&   name,
		const real           defaultValue = 0.0_r,
		const DataTreatment& treatment    = DataTreatment()) const
	-> real;

	auto getVector3(
		const std::string&     name,
		const math::Vector3R&  defaultValue = math::Vector3R(0),
		const DataTreatment&   treatment    = DataTreatment()) const
	-> math::Vector3R;

	auto getQuaternion(
		const std::string&       name,
		const math::QuaternionR& defaultValue = math::QuaternionR::makeNoRotation(),
		const DataTreatment&     treatment    = DataTreatment()) const
	-> math::QuaternionR;

	auto getRealArray(
		const std::string&       name, 
		const std::vector<real>& defaultValue = std::vector<real>(),
		const DataTreatment&     treatment    = DataTreatment()) const
	-> std::vector<real>;

	auto getVector3Array(
		const std::string&                 name, 
		const std::vector<math::Vector3R>& defaultValue = std::vector<math::Vector3R>(),
		const DataTreatment&               treatment    = DataTreatment()) const 
	-> std::vector<math::Vector3R>;

	// Get the string as if the string is a SDL resource identifier and convert
	// it to a path.
	//
	auto getStringAsPath(
		const std::string&   name, 
		const Path&          defaultPath = Path("/"), 
		const DataTreatment& treatment   = DataTreatment()) const 
	-> Path;

	template<typename T>
	auto get(
		const std::string&   dataName,
		const DataTreatment& treatment = DataTreatment()) const 
	-> std::shared_ptr<T>;

	bool hasString(const std::string& name) const;
	bool hasInteger(const std::string& name) const;
	bool hasReal(const std::string& name) const;
	bool hasVector3(const std::string& name) const;
	bool hasQuaternion(const std::string& name) const;

	template<typename T>
	bool hasReference(const std::string& name) const;

	template<typename T>
	std::shared_ptr<T> getCore(const DataTreatment& treatment = DataTreatment()) const;

	bool isPrototypeMatched(const InputPrototype& prototype) const;

	// forbid copying
	InputPacket(const InputPacket& other) = delete;
	InputPacket& operator = (const InputPacket& rhs) = delete;

private:
	const std::vector<ValueClause>    m_vClauses;
	const NamedResourceStorage* const m_storage;
	const Path                        m_workingDirectory;
	const ValueParser                 m_valueParser;

	bool findStringValue(std::string_view typeName, const std::string& dataName, const DataTreatment& treatment,
	                     std::string* const out_value) const;
	Path sdlResourceIdentifierToPath(const std::string& sdlResourceIdentifier) const;

	static void reportDataNotFound(std::string_view typeName, const std::string& name, const DataTreatment& treatment);
	static std::string getCoreDataName();
};

// template implementations:

template<typename T>
std::shared_ptr<T> InputPacket::get(const std::string& dataName, const DataTreatment& treatment) const
{
	const SdlTypeInfo& typeInfo = T::ciTypeInfo();
	std::string resourceName;
	return findStringValue(typeInfo.getCategoryName(), dataName, treatment, &resourceName) ?
	                       m_storage->getResource<T>(resourceName, treatment) : nullptr;
}

template<typename T>
std::shared_ptr<T> InputPacket::getCore(const DataTreatment& treatment) const
{
	return get<T>(getCoreDataName(), treatment);
}

template<typename T>
bool InputPacket::hasReference(const std::string& name) const
{
	const SdlTypeInfo& typeInfo = T::ciTypeInfo();
	return findStringValue(typeInfo.getCategoryName(), name, DataTreatment(), nullptr);
}

}// end namespace ph
