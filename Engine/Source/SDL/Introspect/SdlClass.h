#pragma once

#include "Common/logging.h"
#include "Common/primitive_type.h"
#include "SDL/SdlInputClauses.h"
#include "SDL/SdlOutputClauses.h"
#include "SDL/ESdlTypeCategory.h"

#include <vector>
#include <cstddef>
#include <string>
#include <string_view>
#include <type_traits>

namespace ph { class SdlField; }
namespace ph { class SdlFunction; }
namespace ph { class SdlInputContext; }
namespace ph { class SdlOutputContext; }
namespace ph { class ISdlResource; }

namespace ph
{

PH_DEFINE_EXTERNAL_LOG_GROUP(SdlClass, SDL);

class SdlClass
{
public:
	SdlClass(ESdlTypeCategory category, const std::string& typeName);
	virtual ~SdlClass() = default;

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

	virtual void saveResource(
		const ISdlResource&     resource,
		SdlOutputClauses&       clauses,
		const SdlOutputContext& ctx) const = 0;

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
		const ISdlResource* targetResource,
		std::vector<const ISdlResource*>& out_resources) const = 0;

	/*!
	@return Number of fields directly contained in this class.
	Fields from any associated classes are not accounted for.
	*/
	virtual std::size_t numFields() const = 0;

	virtual const SdlField* getField(std::size_t index) const = 0;

	/*!
	@return Number of functions directly contained in this class.
	Functions from any associated classes are not accounted for.
	*/
	virtual std::size_t numFunctions() const = 0;

	virtual const SdlFunction* getFunction(std::size_t index) const = 0;

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

	std::string genPrettyName() const;
	std::string genCategoryName() const;
	ESdlTypeCategory getCategory() const;
	const std::string& getTypeName() const;
	const std::string& getDocName() const;
	const std::string& getDescription() const;
	const SdlClass* getBase() const;
	bool isDerived() const;
	bool hasField() const;
	bool hasFunction() const;

protected:
	SdlClass& setDescription(std::string description);

	SdlClass& setDocName(std::string docName);

	/*! @brief Set another SDL class as the base of this class.

	The effect of the base class depends on the implementation.
	*/
	template<typename SdlResourceType>
	SdlClass& setBase();

	SdlClass& setIsBlueprint(bool isBlueprint);
	SdlClass& setAllowCreateFromClass(bool allowCreateFromClass);

private:
	ESdlTypeCategory m_category;
	std::string m_typeName;
	std::string m_docName;
	std::string m_description;
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

inline const std::string& SdlClass::getTypeName() const
{
	return m_typeName;
}

inline const std::string& SdlClass::getDocName() const
{
	return m_docName;
}

inline const std::string& SdlClass::getDescription() const
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
