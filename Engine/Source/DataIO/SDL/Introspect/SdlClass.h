#pragma once

#include "Common/assertion.h"
#include "Common/Logger.h"

#include <vector>
#include <memory>
#include <cstddef>
#include <string>
#include <utility>

namespace ph
{

class SdlField;
class SdlInputContext;
class ISdlResource;
class ValueClause;

class SdlClass
{
public:
	SdlClass(std::string category, std::string typeName);
	virtual ~SdlClass() = default;

	virtual std::shared_ptr<ISdlResource> createResource() const = 0;

	virtual void initResource(
		ISdlResource&          resource,
		const ValueClause*     clauses,
		std::size_t            numClauses,
		const SdlInputContext& ctx) const = 0;

	// TODO: saveToSdl()

	virtual std::size_t numFields() const = 0;
	virtual const SdlField* getField(std::size_t index) const = 0;

	std::string genPrettyName() const;
	const std::string& getCategory() const;
	const std::string& getTypeName() const;
	const std::string& getDescription() const;
	const SdlClass* getBase() const;
	bool isDerived() const;

	SdlClass& setDescription(std::string description);
	SdlClass& setBase(const SdlClass* base);

protected:
	static const Logger logger;

private:
	std::string m_category;
	std::string m_typeName;
	std::string m_description;

	const SdlClass* m_base;
};

// In-header Implementation:

inline SdlClass::SdlClass(std::string category, std::string typeName) :
	m_category   (std::move(category)), 
	m_typeName   (std::move(typeName)),
	m_description(),
	m_base       (nullptr)
{
	PH_ASSERT(!m_category.empty());
	PH_ASSERT(!m_typeName.empty());
}

inline const std::string& SdlClass::getCategory() const
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

inline SdlClass& SdlClass::setBase(const SdlClass* const base)
{
	m_base = base;

	return *this;
}

}// end namespace ph
