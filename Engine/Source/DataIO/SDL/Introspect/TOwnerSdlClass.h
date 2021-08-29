#pragma once

#include "DataIO/SDL/Introspect/SdlClass.h"
#include "DataIO/SDL/Introspect/TOwnedSdlField.h"
#include "DataIO/SDL/Introspect/TBasicSdlFieldSet.h"
#include "Utility/TArrayAsVector.h"
#include "Common/config.h"

#include <string>
#include <utility>
#include <string_view>
#include <array>
#include <type_traits>

namespace ph
{

class ValueClauses;
class SdlInputContext;
class SdlStructFieldStump;

/*! @brief SDL binding type for a canonical SDL resource class.
*/
template<typename Owner, typename FieldSet = TBasicSdlFieldSet<TOwnedSdlField<Owner>>>
class TOwnerSdlClass : public SdlClass
{
public:
	using OwnerType = Owner;

public:
	explicit TOwnerSdlClass(std::string displayName);

	std::shared_ptr<ISdlResource> createResource() const override;

	void initResource(
		ISdlResource&          resource,
		ValueClauses&          clauses,
		const SdlInputContext& ctx) const override;

	void saveResource(
		const ISdlResource&     resource,
		OutputPayloads&         payloads,
		const SdlOutputContext& ctx) const override;

	void call(
		std::string_view       funcName,
		ISdlResource*          resource,
		ValueClauses&          clauses,
		const SdlInputContext& ctx) const override;

	void associatedResources(
		const ISdlResource& targetResource,
		std::vector<const ISdlResource*>& out_resources) const override;

	std::size_t numFields() const override;
	const SdlField* getField(std::size_t index) const override;
	std::size_t numFunctions() const override;
	const SdlFunction* getFunction(std::size_t index) const override;

	bool isBlueprint() const override;

	/*!
	Loads only fields in this class. Does *not* take other indirect fields into account 
	(such as fields in base class).
	*/
	void loadFieldsFromSdl(
		Owner&                 owner,
		ValueClauses&          clauses,
		const SdlInputContext& ctx) const;

	/*!
	Saves only fields in this class. Does *not* take other indirect fields into account
	(such as fields in base class).
	*/
	void saveFieldsToSdl(
		const Owner&            owner,
		OutputPayloads&         payloads,
		const SdlOutputContext& ctx) const;

	const TOwnedSdlField<Owner>* getOwnedField(std::size_t index) const;

	template<typename SdlFieldType>
	TOwnerSdlClass& addField(SdlFieldType sdlField);

	template<typename StructType>
	TOwnerSdlClass& addStruct(StructType Owner::* structObjPtr);

	template<typename StructType>
	TOwnerSdlClass& addStruct(
		StructType Owner::*        structObjPtr,
		const SdlStructFieldStump& structFieldStump);

	/*! @brief Adds a function that can later be called.
	*/
	template<typename T>
	TOwnerSdlClass& addFunction();

	auto description(std::string descriptionStr) -> TOwnerSdlClass&;

	auto docName(std::string docName) -> TOwnerSdlClass&;

	/*! @brief Set another SDL class as the base of this class.

	By default, fields and functions in the base are automatically inherited.
	*/
	template<typename T>
	auto baseOn() -> TOwnerSdlClass&;

private:
	using FunctionSet = TArrayAsVector<const SdlFunction*, PH_SDL_MAX_FUNCTIONS>;

	FieldSet    m_fields;
	FunctionSet m_functions;
};

}// end namespace ph

#include "DataIO/SDL/Introspect/TOwnerSdlClass.ipp"
