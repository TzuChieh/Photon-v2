#pragma once

#include "Common/logging.h"

#include <cstddef>
#include <string>

namespace ph
{

PH_DEFINE_EXTERNAL_LOG_GROUP(SdlStruct, SDL);

class SdlField;
class ValueClause;
class SdlInputContext;

class SdlStruct
{
public:
	explicit SdlStruct(std::string typeName);
	virtual ~SdlStruct() = default;

	virtual std::size_t numFields() const = 0;
	virtual const SdlField* getField(std::size_t index) const = 0;

	const std::string& getTypeName() const;
	const std::string& getDescription() const;

protected:
	SdlStruct& setDescription(std::string description);

private:
	std::string m_typeName;
	std::string m_description;
};

// In-header Implementations:

inline const std::string& SdlStruct::getTypeName() const
{
	return m_typeName;
}

inline const std::string& SdlStruct::getDescription() const
{
	return m_description;
}

}// end namespace ph
