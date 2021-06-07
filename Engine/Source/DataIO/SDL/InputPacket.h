#pragma once

#include "Common/primitive_type.h"
#include "Common/assertion.h"
#include "DataIO/SDL/ValueClause.h"
#include "Math/TVector3.h"
#include "Math/TQuaternion.h"
#include "DataIO/SDL/DataTreatment.h"
#include "DataIO/SDL/SdlTypeInfo.h"
#include "DataIO/SDL/SceneDescription.h"
#include "DataIO/FileSystem/Path.h"
#include "DataIO/SDL/ValueParser.h"
#include "Utility/IMoveOnly.h"

#include <vector>
#include <string>
#include <memory>
#include <string_view>
#include <type_traits>
#include <utility>

namespace ph
{

class InputPrototype;

class InputPacket final : public IMoveOnly
{
private:
	template<typename ValueType>
	static auto makeDefaultValue() -> ValueType;

	template<typename DataType>
	static constexpr bool isReference() noexcept;

public:
	InputPacket(
		const std::vector<ValueClause>& vClauses, 
		const SceneDescription*         scene,
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
	auto getStringAsPath(
		const std::string&   name, 
		const Path&          defaultPath = Path("/"), 
		const DataTreatment& treatment   = DataTreatment()) const 
	-> Path;

	bool hasString(const std::string& name) const;
	bool hasInteger(const std::string& name) const;
	bool hasReal(const std::string& name) const;
	bool hasVector3(const std::string& name) const;
	bool hasQuaternion(const std::string& name) const;

	template<typename RefType>
	bool hasReference(const std::string& name) const;

	bool isPrototypeMatched(const InputPrototype& prototype) const;

	template<typename ValType>
	auto getValue(
		std::string_view     valueName,
		ValType              defaultValue = makeDefaultValue<ValType>(),
		const DataTreatment& treatment    = DataTreatment()) const
	-> ValType;

	template<typename RefType>
	auto getReference(
		const std::string&   referenceName,
		const DataTreatment& treatment = DataTreatment()) const
	-> std::shared_ptr<RefType>;

	template<typename ValType, typename = std::enable_if_t<!isReference<ValType>, std::false_type>>
	auto get(
		const std::string_view valueName,
		ValType                defaultValue = makeDefaultValue<ValType>(),
		const DataTreatment&   treatment = DataTreatment()) const
	-> ValType
	{
		return getValue<ValType>(
			valueName,
			std::move(defaultValue),
			treatment);
	}

	template<typename RefType, typename = std::enable_if_t<isReference<RefType>, std::true_type>>
	auto get(
		const std::string_view referenceName,
		const DataTreatment&   treatment = DataTreatment()) const
	-> std::shared_ptr<RefType>
	{
		return getReference<RefType>(
			referenceName,
			treatment);
	}

private:
	const std::vector<ValueClause> m_vClauses;
	const SceneDescription* const  m_scene;
	const Path                     m_workingDirectory;
	const ValueParser              m_valueParser;

	bool findStringValue(std::string_view typeName, const std::string& dataName, const DataTreatment& treatment,
	                     std::string* const out_value) const;
	Path sdlResourceIdentifierToPath(const std::string& sdlResourceIdentifier) const;

	static void reportDataNotFound(std::string_view typeName, const std::string& name, const DataTreatment& treatment);
};

// template implementations:

template<typename RefType>
inline auto InputPacket::getReference(
	const std::string&   referenceName, 
	const DataTreatment& treatment) const
-> std::shared_ptr<RefType>
{
	PH_ASSERT(isReference<RefType>());

	const SdlTypeInfo& typeInfo = RefType::ciTypeInfo();
	std::string resourceName;
	return findStringValue(typeInfo.getCategoryName(), referenceName, treatment, &resourceName) ?
	                       m_scene->getResource<RefType>(resourceName, treatment) : nullptr;
}

template<typename RefType>
inline bool InputPacket::hasReference(const std::string& name) const
{
	PH_ASSERT(isReference<RefType>());

	const SdlTypeInfo& typeInfo = RefType::ciTypeInfo();
	return findStringValue(typeInfo.getCategoryName(), name, DataTreatment(), nullptr);
}

template<typename ValType>
inline auto InputPacket::getValue(
	const std::string_view valueName,
	ValType                defaultValue,
	const DataTreatment&   treatment) const 
-> ValType
{
	PH_ASSERT(!isReference<ValType>());

	if constexpr(std::is_same_v<ValType, std::string>)
	{
		return getString(std::string(valueName), std::move(defaultValue), treatment);
	}

	if constexpr(std::is_same_v<ValType, integer>)
	{
		return getInteger(std::string(valueName), std::move(defaultValue), treatment);
	}

	if constexpr(std::is_same_v<ValType, real>)
	{
		return getReal(std::string(valueName), std::move(defaultValue), treatment);
	}

	if constexpr(std::is_same_v<ValType, math::Vector3R>)
	{
		return getVector3(std::string(valueName), std::move(defaultValue), treatment);
	}

	if constexpr(std::is_same_v<ValType, math::QuaternionR>)
	{
		return getQuaternion(std::string(valueName), std::move(defaultValue), treatment);
	}

	if constexpr(std::is_same_v<ValType, std::vector<real>>)
	{
		return getRealArray(std::string(valueName), std::move(defaultValue), treatment);
	}

	if constexpr(std::is_same_v<ValType, std::vector<math::Vector3R>>)
	{
		return getVector3Array(std::string(valueName), std::move(defaultValue), treatment);
	}

	PH_ASSERT_UNREACHABLE_SECTION();
	return std::move(defaultValue);
}

template<typename ValType>
inline auto InputPacket::makeDefaultValue()
	-> ValType
{
	if constexpr(std::is_same_v<ValType, std::string>)
	{
		return "";
	}

	if constexpr(std::is_same_v<ValType, integer>)
	{
		return 0;
	}

	if constexpr(std::is_same_v<ValType, real>)
	{
		return 0.0_r;
	}

	if constexpr(std::is_same_v<ValType, math::Vector3R>)
	{
		return math::Vector3R(0);
	}

	if constexpr(std::is_same_v<ValType, math::QuaternionR>)
	{
		return math::QuaternionR::makeNoRotation();
	}

	if constexpr(std::is_same_v<ValType, std::vector<real>>)
	{
		return {};
	}

	if constexpr(std::is_same_v<ValType, std::vector<math::Vector3R>>)
	{
		return {};
	}

	PH_ASSERT_UNREACHABLE_SECTION();
	return ValType();
}

template<typename DataType>
inline constexpr bool InputPacket::isReference() noexcept
{
	return std::is_base_of_v<ISdlResource, DataType>;
}

}// end namespace ph
