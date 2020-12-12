#pragma once

#include "DataIO/SDL/Introspect/SdlField.h"

#include <utility>

namespace ph
{

template<typename Owner>
class TOwnedSdlField : public SdlField
{
public:
	TOwnedSdlField(std::string typeName, std::string valueName);

	virtual bool fromSdl(Owner& owner, const std::string& sdl) = 0;
	virtual void toSdl(Owner& owner, std::string* out_sdl) const = 0;
};

// In-header Implementations:

template<typename Owner>
inline TOwnedSdlField<Owner>::TOwnedSdlField(std::string typeName, std::string valueName) : 
	SdlField(std::move(typeName), std::move(valueName))
{}

}// end namespace ph