#pragma once

#include "Engine/SDL/Introspect/ISdlInstantiable.h"
#include "Engine/SDL/sdl_fwd.h"
#include "Engine/SDL/SdlInputClauses.h"
#include "Engine/SDL/SdlOutputClauses.h"
#include "Engine/SDL/ESdlTypeCategory.h"
#include "Engine/SDL/Definition/SdlUserSpec.h"

#include <Common/primitive_type.h>
#include <Common/logging.h>

#include <vector>
#include <cstddef>
#include <string>
#include <string_view>
#include <type_traits>
#include <memory>

namespace ph
{

PH_DEFINE_EXTERNAL_LOG_GROUP(SdlClass, SDL);

class SdlClass : public ISdlInstantiable
{
public:
	SdlClass();

	/*! @brief Create a resource instance of this class.
	@return A new resource instance. Can be null if the class is a blueprint (for example) or an error occurs.
	*/
	virtual std::shared_ptr<ISdlResource> createResource() const = 0;

	/*! @brief Initialize a resource from value clauses.
	How the resource will be initialized depends on the resource class's SDL definition.
	*/
	virtual void initResource(
		ISdlResource&          resource,
		SdlInputClauses&       clauses,
		const SdlInputContext& ctx) const = 0;

	/*! @brief Initialize a resource to default values.
	Default values are defined by the resource class's SDL definition.
	*/
	virtual void initDefaultResource(ISdlResource& resource) const = 0;

	/*! @brief Save a resource instance to value clauses.
	@param resource The resource instance to save.
	@param clauses Output buffer for storing the saved values.
	@param ctx Context for the saving operation.
	*/
	virtual void saveResource(
		const ISdlResource&     resource,
		SdlOutputClauses&       clauses,
		const SdlOutputContext& ctx) const = 0;

	/*! @brief Call a function of the class.
	@param funcName Name of the function to call.
	@param resource The target resource instance. Can be null if the function is static.
	@param clauses Input clauses for the function call.
	@param ctx Context for the function call.
	*/
	virtual void call(
		std::string_view       funcName,
		ISdlResource*          resource,
		SdlInputClauses&       clauses,
		const SdlInputContext& ctx) const = 0;

	/*! @brief Get all SDL resources referenced by @p targetResource.
	@param targetResource Resource instance in question.
	@param out_resources SDL resources referenced by @p targetResource. Never contains null.
	Appends to existing ones.
	*/
	virtual void referencedResources(
		const ISdlResource& targetResource,
		std::vector<const ISdlResource*>& out_resources) const = 0;

	/*!
	@return Number of functions directly contained in this class.
	Functions from any associated classes are not accounted for.
	*/
	virtual std::size_t numFunctions() const = 0;

	/*! @brief Get a function of the class by index.
	@param index The index of the function to retrieve. Must be less than `numFunctions()`.
	*/
	virtual const SdlFunction* getFunction(std::size_t index) const = 0;

	/*!
	@return Number of fields directly contained in this class.
	Fields from any associated classes are not accounted for.
	*/
	std::size_t numFields() const override = 0;

	/*! @brief Get a field of the class by index.
	@param index The index of the field to retrieve. Must be less than `numFields()`.
	*/
	const SdlField* getField(std::size_t index) const override = 0;

	SdlInstantiated instantiate() const override;
	std::string_view getTypeName() const override;
	std::string_view getDescription() const override;

	/*! @brief Whether the class is for building other classes only.
	A blueprint class cannot be instantiated as a SDL resource. Note that blueprint class is semantically
	different to abstract class; nevertheless, they often are closely related.
	*/
	bool isBlueprint() const;

	/*! @brief Whether a resource can be created by calling createResource().
	This attribute is useful to decide whether the resource need custom construction routines to create
	them. A true return value does not mean createResource() always return a valid resource, e.g.,
	a blueprint class or some error occurs.
	*/
	bool allowCreateFromClass() const;

	/*! @brief Generate a human-readable name for the class.
	*/
	std::string genPrettyName() const;

	/*! @brief Generate a human-readable name for the class's category.
	*/
	std::string genCategoryName() const;

	/*! @brief Get the category of the class.
	*/
	ESdlTypeCategory getCategory() const;
	
	/*! @brief Get the user specifications for the class.
	*/
	const SdlUserSpec& getUserSpec() const;

	/*! @brief Get the documentation name for the class.
	*/
	std::string_view getDocName() const;

	/*! @brief Get the base class of this class.
	@return Pointer to the base SDL class, or null if no base class exists.
	*/
	const SdlClass* getBase() const;

	/*! @brief Whether the class is derived from another SDL class.
	*/
	bool isDerived() const;

	/*! @brief Whether the class contains any SDL fields.
	*/
	bool hasField() const;

	/*! @brief Whether the class contains any SDL functions.
	*/
	bool hasFunction() const;

protected:
	/*! @brief Set the user specifications for the class.
	@return `*this` for chaining.
	*/
	SdlClass& setUserSpec(SdlUserSpec spec);

	/*! @brief Set the category and type name of the class.
	@return `*this` for chaining.
	*/
	SdlClass& setTypeInfo(ESdlTypeCategory category, std::string typeName);

	/*! @brief Set the description of the class.
	@return `*this` for chaining.
	*/
	SdlClass& setDescription(std::string description);

	/*! @brief Set the documentation name of the class.
	@return `*this` for chaining.
	*/
	SdlClass& setDocName(std::string docName);

	/*! @brief Set another SDL class as the base of this class.
	The effect of the base class depends on the implementation.
	@return `*this` for chaining.
	*/
	template<typename SdlResourceType>
	SdlClass& setBase();

	/*! @brief Set whether the class is a blueprint class.
	@return `*this` for chaining.
	*/
	SdlClass& setIsBlueprint(bool isBlueprint);

	/*! @brief Set whether the resource can be created from this class.
	@return `*this` for chaining.
	*/
	SdlClass& setAllowCreateFromClass(bool allowCreateFromClass);

private:
	ESdlTypeCategory m_category;
	std::string m_typeName;
	std::string m_docName;
	std::string m_description;
	SdlUserSpec m_userSpec;
	const SdlClass* m_base;
	uint32 m_isBlueprint : 1;
	uint32 m_allowCreateFromClass : 1;
};

// In-header Implementation:

inline bool SdlClass::isBlueprint() const
{
	return m_isBlueprint;
}

inline bool SdlClass::allowCreateFromClass() const
{
	return m_allowCreateFromClass;
}

inline ESdlTypeCategory SdlClass::getCategory() const
{
	return m_category;
}

inline std::string_view SdlClass::getTypeName() const
{
	return m_typeName;
}

inline const SdlUserSpec& SdlClass::getUserSpec() const
{
	return m_userSpec;
}

inline std::string_view SdlClass::getDocName() const
{
	return m_docName;
}

inline std::string_view SdlClass::getDescription() const
{
	return m_description;
}

inline const SdlClass* SdlClass::getBase() const
{
	return m_base;
}

inline bool SdlClass::isDerived() const
{
	return getBase() != nullptr;
}

inline bool SdlClass::hasField() const
{
	return numFields() != 0;
}

inline bool SdlClass::hasFunction() const
{
	return numFunctions() != 0;
}

template<typename SdlResourceType>
inline SdlClass& SdlClass::setBase()
{
	static_assert(std::is_base_of_v<ISdlResource, SdlResourceType>,
		"Input type must be a SDL resource.");

	m_base = SdlResourceType::getSdlClass();
	return *this;
}

}// end namespace ph
