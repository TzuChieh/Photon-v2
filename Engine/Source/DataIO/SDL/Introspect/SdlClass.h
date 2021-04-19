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
	SdlClass(std::string category, std::string displayName);
	virtual ~SdlClass() = default;

	virtual void initFromSdl(
		ISdlResource& resource,
		const ValueClause* clauses,
		std::size_t        numClauses,
		SdlInputContext&   ctx) const = 0;

	// TODO: saveToSdl()

	virtual std::size_t numFields() const = 0;
	virtual const SdlField* getField(std::size_t index) const = 0;

	std::string genPrettyName() const;
	const std::string& getCategory() const;
	const std::string& getDisplayName() const;
	const std::string& getDescription() const;

	SdlClass& setDescription(std::string description);

protected:
	static const Logger logger;

private:
	std::string m_category;
	std::string m_displayName;
	std::string m_description;
};

// In-header Implementation:

inline SdlClass::SdlClass(std::string category, std::string displayName) : 
	m_category(std::move(category)), 
	m_displayName(std::move(displayName)),
	m_description()
{
	PH_ASSERT(!category.empty());
	PH_ASSERT(!displayName.empty());
}

inline const std::string& SdlClass::getCategory() const
{
	return m_category;
}

inline const std::string& SdlClass::getDisplayName() const
{
	return m_displayName;
}

inline const std::string& SdlClass::getDescription() const
{
	return m_description;
}

inline SdlClass& SdlClass::setDescription(std::string description)
{
	m_description = std::move(description);

	return *this;
}

}// end namespace ph
