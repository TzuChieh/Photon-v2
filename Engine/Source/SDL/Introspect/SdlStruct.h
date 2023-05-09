#pragma once

#include "Common/logging.h"

#include <cstddef>
#include <string>

namespace ph
{

PH_DEFINE_EXTERNAL_LOG_GROUP(SdlStruct, SDL);

class SdlField;
class SdlInputClauses;
class SdlInputContext;

/*!
SDL struct do not provide interface for initializing instances to default values such as the one
provided by SDL class (`SdlClass::initDefaultResource()`). The rationale behind this is that SDL
struct is designed to be a simple grouped data carrier without the burden of inheritance and 
member functions. For efficiency and ease of reuse, initializing instances to default values 
requires a concrete type (the exact type of the struct). Implementations that wish to support this
feature should define a method with signature `initDefaultStruct(T& instance) const` where T is
the concrete type of the struct.
*/
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
