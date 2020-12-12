#pragma once

#include "DataIO/SDL/Introspect/TOwnedSdlField.h"
#include "Common/assertion.h"

#include <utility>

namespace ph
{

template<typename T, typename Owner>
class TSdlValue : public TOwnedSdlField<Owner>
{
public:
	TSdlValue(
		std::string typeName, 
		std::string valueName, 
		T Owner::*  valuePtr);

	bool fromSdl(Owner& owner, const std::string& sdl) override = 0;
	void toSdl(Owner& owner, std::string* out_sdl) const override = 0;

	TSdlValue& defaultTo(T defaultValue);

protected:
	T Owner::* m_valuePtr;
	T          m_defaultValue;
};

// In-header Implementations:

template<typename T, typename Owner>
inline TSdlValue<T, Owner>::TSdlValue(
	std::string      typeName, 
	std::string      valueName, 
	T Owner::* const valuePtr) : 

	TOwnedSdlField<Owner>(std::move(typeName), std::move(valueName)),

	m_valuePtr    (valuePtr),
	m_defaultValue(0)
{
	PH_ASSERT(m_valuePtr);
}

}// end namespace ph
