#pragma once

#include "SDL/Introspect/ISdlInstantiable.h"
#include "SDL/sdl_fwd.h"
#include "Common/logging.h"

#include <cstddef>
#include <string>

namespace ph
{

PH_DEFINE_EXTERNAL_LOG_GROUP(SdlStruct, SDL);

/*!
SDL struct do not provide interface for initializing instances to default values such as the one
provided by SDL class (`SdlClass::initDefaultResource()`). The rationale behind this is that SDL
struct is designed to be a simple grouped data carrier without the burden of inheritance and 
member functions. For efficiency and ease of reuse, initializing instances to default values 
requires a concrete type (the exact type of the struct). Implementations that wish to support this
feature should use define a method with signature `initDefaultStruct(T& instance) const` where T is
the concrete type of the struct. 

`TSdl` provides a simplified interface for initializing instances to default values.
*/
class SdlStruct : public ISdlInstantiable
{
public:
	explicit SdlStruct(std::string typeName);

	virtual std::size_t numFields() const = 0;
	virtual const SdlField* getField(std::size_t index) const = 0;

	std::string_view getTypeName() const override;
	std::string_view getDescription() const override;

protected:
	SdlStruct& setDescription(std::string description);

private:
	std::string m_typeName;
	std::string m_description;
};

// In-header Implementations:

inline std::string_view SdlStruct::getTypeName() const
{
	return m_typeName;
}

inline std::string_view SdlStruct::getDescription() const
{
	return m_description;
}

}// end namespace ph
