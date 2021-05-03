#pragma once

#include "Common/assertion.h"

#include <cstddef>
#include <string>
#include <utility>

namespace ph
{

class SdlField;
class ValueClause;
class SdlInputContext;

class SdlStruct
{
public:
	explicit SdlStruct(std::string name);
	virtual ~SdlStruct() = default;

	virtual void initFields(
		const ValueClause*     clauses,
		std::size_t            numClauses,
		const SdlInputContext& ctx) const = 0;

	virtual std::size_t numFields() const = 0;
	virtual const SdlField* getField(std::size_t index) const = 0;

	const std::string& getName() const;
	const std::string& getDescription() const;

	SdlStruct& setDescription(std::string description);

private:
	std::string m_name;
	std::string m_description;
};

// In-header Implementations:

inline SdlStruct::SdlStruct(std::string name) : 
	m_name       (std::move(name)),
	m_description()
{
	PH_ASSERT(!m_name.empty());
}

inline const std::string& SdlStruct::getName() const
{
	return m_name;
}

inline const std::string& SdlStruct::getDescription() const
{
	return m_description;
}

inline SdlStruct& SdlStruct::setDescription(std::string description)
{
	m_description = std::move(description);

	return *this;
}

}// end namespace ph
