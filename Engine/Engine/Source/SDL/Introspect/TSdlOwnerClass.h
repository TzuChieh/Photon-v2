#pragma once

#include "SDL/Introspect/SdlClass.h"
#include "SDL/sdl_fwd.h"
#include "SDL/Introspect/TSdlOwnedField.h"
#include "SDL/Introspect/TSdlBruteForceFieldSet.h"
#include "Utility/TArrayVector.h"

#include <Common/config.h>

#include <string>
#include <utility>
#include <string_view>
#include <array>

namespace ph
{

/*! @brief SDL binding type for a canonical SDL resource class.
*/
template<typename Owner, typename FieldSet = TSdlBruteForceFieldSet<TSdlOwnedField<Owner>>>
class TSdlOwnerClass : public SdlClass
{
public:
	using OwnerType = Owner;

public:
	explicit TSdlOwnerClass(std::string displayName);

	std::shared_ptr<ISdlResource> createResource() const override;

	void initResource(
		ISdlResource&          resource,
		SdlInputClauses&       clauses,
		const SdlInputContext& ctx) const override;

	void initDefaultResource(ISdlResource& resource) const override;

	void saveResource(
		const ISdlResource&     resource,
		SdlOutputClauses&       out_clauses,
		const SdlOutputContext& ctx) const override;

	void call(
		std::string_view       funcName,
		ISdlResource*          resource,
		SdlInputClauses&       clauses,
		const SdlInputContext& ctx) const override;

	void referencedResources(
		const ISdlResource* targetResource,
		std::vector<const ISdlResource*>& out_resources) const override;

	std::size_t numFields() const override;
	const SdlField* getField(std::size_t index) const override;
	std::size_t numFunctions() const override;
	const SdlFunction* getFunction(std::size_t index) const override;

	/*!
	Loads only fields in this class. Does *not* take other indirect fields into account 
	(such as fields in base class).
	*/
	void loadFieldsFromSdl(
		Owner&                 owner,
		SdlInputClauses&       clauses,
		const SdlInputContext& ctx) const;

	/*!
	Saves only fields in this class. Does *not* take other indirect fields into account
	(such as fields in base class).
	*/
	void saveFieldsToSdl(
		const Owner&            owner,
		SdlOutputClauses&       out_clauses,
		const SdlOutputContext& ctx) const;

	const TSdlOwnedField<Owner>* getOwnedField(std::size_t index) const;

	template<typename SdlFieldType>
	TSdlOwnerClass& addField(SdlFieldType sdlField);

	template<typename StructType>
	TSdlOwnerClass& addStruct(StructType Owner::* structObjPtr);

	template<typename StructType>
	TSdlOwnerClass& addStruct(
		StructType Owner::*        structObjPtr,
		const SdlStructFieldStump& structFieldStump);

	/*! @brief Adds a function that can later be called.
	*/
	template<typename T>
	TSdlOwnerClass& addFunction();

	auto description(std::string descriptionStr) -> TSdlOwnerClass&;

	auto docName(std::string docName) -> TSdlOwnerClass&;

	/*! @brief Set another SDL class as the base of this class.

	By default, fields and functions in the base are automatically inherited.
	*/
	template<typename T>
	auto baseOn() -> TSdlOwnerClass&;

	using SdlClass::allowCreateFromClass;
	auto allowCreateFromClass(bool allowCreateFromClass) -> TSdlOwnerClass&;

private:
	/*!
	Set fields of the instance @p owner to default values (as described in SDL class definition).
	Note that fields in base class are not set.
	*/
	void setFieldsToDefaults(Owner& owner) const;

	/*!
	Cast input instance to another type, possibly `const` qualified.
	@return The casted-to instance. Never null.
	*/
	template<typename DstType, typename SrcType>
	DstType* castTo(SrcType* srcInstance) const;

	using FunctionSet = TArrayVector<const SdlFunction*, PH_SDL_MAX_FUNCTIONS>;

	FieldSet    m_fields;
	FunctionSet m_functions;
};

}// end namespace ph

#include "SDL/Introspect/TSdlOwnerClass.ipp"
