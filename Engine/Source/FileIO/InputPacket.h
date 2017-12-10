#pragma once

#include "Common/primitive_type.h"
#include "FileIO/ValueClause.h"
#include "Math/TVector3.h"
#include "Math/TQuaternion.h"
#include "FileIO/DataTreatment.h"
#include "FileIO/SDL/SdlTypeInfo.h"
#include "FileIO/NamedResourceStorage.h"
#include "FileIO/FileSystem/Path.h"

#include <vector>
#include <string>
#include <memory>

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
	
	std::string getString(
		const std::string&   name, 
		const std::string&   defaultString = "", 
		const DataTreatment& treatment     = DataTreatment()) const;

	integer getInteger(
		const std::string&   name, 
		const integer        defaultInteger = 0, 
		const DataTreatment& treatment      = DataTreatment()) const;

	real getReal(
		const std::string&   name, 
		const real           defaultReal = 0.0f, 
		const DataTreatment& treatment   = DataTreatment()) const;

	Vector3R getVector3r(
		const std::string&   name, 
		const Vector3R&      defaultVector3r = Vector3R(), 
		const DataTreatment& treatment       = DataTreatment()) const;

	QuaternionR getQuaternionR(
		const std::string&   name,
		const QuaternionR&   defaultQuaternionR = QuaternionR(),
		const DataTreatment& treatment          = DataTreatment()) const;

	std::vector<Vector3R> getVector3rArray(
		const std::string&           name, 
		const std::vector<Vector3R>& defaultVector3rArray = std::vector<Vector3R>(), 
		const DataTreatment&         treatment            = DataTreatment()) const;

	// Get string as path if the string is a SDL resource identifier.
	Path getStringAsPath(
		const std::string&   name, 
		const Path&          defaultPath = Path("/"), 
		const DataTreatment& treatment   = DataTreatment()) const;

	template<typename T>
	std::shared_ptr<T> get(
		const std::string&   dataName,
		const DataTreatment& treatment = DataTreatment()) const;

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

	bool findStringValue(const std::string& typeName, const std::string& dataName, const DataTreatment& treatment,
	                     std::string* const out_value) const;

	static void reportDataNotFound(const std::string& typeName, const std::string& name, const DataTreatment& treatment);
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

}// end namespace ph