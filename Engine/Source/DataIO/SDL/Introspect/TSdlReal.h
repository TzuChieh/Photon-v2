#pragma once

#include "DataIO/SDL/Introspect/TSdlValue.h"
#include "Common/primitive_type.h"
#include "Common/assertion.h"

#include <type_traits>
#include <string>
#include <utility>

namespace ph
{

template<typename Owner, typename RealType = real>
class TSdlReal : public TSdlValue<RealType, Owner>
{
	static_assert(std::is_same_v<RealType, real>, 
		"Currently supports only real");

public:
	TSdlReal(std::string valueName, RealType Owner::* valuePtr);

	bool fromSdl(Owner& owner, const std::string& sdl) override;
	void toSdl(Owner& owner, std::string* out_sdl) const override;
};

// In-header Implementations:

template<typename Owner, typename RealType>
inline TSdlReal<Owner, RealType>::TSdlReal(std::string valueName, RealType Owner::* const valuePtr) : 
	TSdlValue<RealType, Owner>("real", std::move(valueName), valuePtr)
{}

template<typename Owner, typename RealType>
inline bool TSdlReal<Owner, RealType>::fromSdl(Owner& owner, const std::string& sdl)
{
	owner.*m_valuePtr = static_cast<real>(std::stold(sdl));
}

template<typename Owner, typename RealType>
inline void TSdlReal<Owner, RealType>::toSdl(Owner& owner, std::string* const out_sdl) const
{
	PH_ASSERT(out_sdl);

	// TODO
	PH_ASSERT_UNREACHABLE_SECTION();
}

}// end namespace ph
