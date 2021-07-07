#pragma once

#include "Common/assertion.h"
#include "Common/Logger.h"
#include "DataIO/SDL/ValueClauses.h"
#include "DataIO/SDL/ETypeCategory.h"
#include "Utility/IMoveOnly.h"

#include <vector>
#include <memory>
#include <cstddef>
#include <string>
#include <utility>
#include <string_view>
#include <type_traits>

namespace ph
{

class SdlField;
class SdlFunction;
class SdlInputContext;
class ISdlResource;

class SdlClass
{
public:
	SdlClass(ETypeCategory category, std::string typeName);
	virtual ~SdlClass() = default;

	virtual std::shared_ptr<ISdlResource> createResource() const = 0;

	virtual void initResource(
		ISdlResource&          resource,
		ValueClauses&          clauses,
		const SdlInputContext& ctx) const = 0;

	virtual void call(
		std::string_view       funcName,
		ISdlResource*          resource,
		ValueClauses&          clauses,
		const SdlInputContext& ctx) const = 0;

	// TODO: saveToSdl()

	virtual std::size_t numFields() const = 0;
	virtual const SdlField* getField(std::size_t index) const = 0;
	virtual std::size_t numFunctions() const = 0;
	virtual const SdlFunction* getFunction(std::size_t index) const = 0;

	std::string genPrettyName() const;
	std::string genCategoryName() const;
	ETypeCategory getCategory() const;
	const std::string& getTypeName() const;
	const std::string& getDescription() const;
	const SdlClass* getBase() const;
	bool isDerived() const;

protected:
	SdlClass& setDescription(std::string description);

	/*! @brief Set another SDL class as the base of this class.

	The effect of the base class depends on the implementation.
	*/
	template<typename SdlResourceType>
	SdlClass& setBase();

	static const Logger logger;

private:
	ETypeCategory m_category;
	std::string   m_typeName;
	std::string   m_description;

	const SdlClass* m_base;
};

// In-header Implementation:

inline SdlClass::SdlClass(const ETypeCategory category, std::string typeName) :
	m_category   (category), 
	m_typeName   (std::move(typeName)),
	m_description(),
	m_base       (nullptr)
{
	PH_ASSERT(!m_typeName.empty());
	PH_ASSERT_MSG(m_category != ETypeCategory::UNSPECIFIED,
		"unspecified SDL resource category detected in " + genPrettyName() + "; "
		"consult documentation of ISdlResource and see if the SDL resource is "
		"properly implemented");
}

inline ETypeCategory SdlClass::getCategory() const
{
	return m_category;
}

inline const std::string& SdlClass::getTypeName() const
{
	return m_typeName;
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

inline SdlClass& SdlClass::setDescription(std::string description)
{
	m_description = std::move(description);
	return *this;
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
