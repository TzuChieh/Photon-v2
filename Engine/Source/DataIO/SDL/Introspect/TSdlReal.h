#pragma once

#include "DataIO/SDL/Introspect/TSdlValue.h"
#include "Common/primitive_type.h"
#include "Common/assertion.h"

#include <type_traits>
#include <string>
#include <utility>
#include <exception>

namespace ph
{

template<typename Owner, typename RealType = real>
class TSdlReal : public TSdlValue<RealType, Owner>
{
	static_assert(std::is_same_v<RealType, real>, 
		"Currently supports only real");

public:
	TSdlReal(std::string valueName, RealType Owner::* valuePtr);

	bool loadFromSdl(
		Owner&             owner,
		const std::string& sdl,
		std::string&       out_loaderMessage) override;

	void convertToSdl(
		Owner&       owner,
		std::string* out_sdl,
		std::string& out_converterMessage) const override;
};

// In-header Implementations:

template<typename Owner, typename RealType>
inline TSdlReal<Owner, RealType>::TSdlReal(std::string valueName, RealType Owner::* const valuePtr) : 
	TSdlValue<RealType, Owner>("real", std::move(valueName), valuePtr)
{}

template<typename Owner, typename RealType>
inline bool TSdlReal<Owner, RealType>::loadFromSdl(
	Owner&             owner,
	const std::string& sdl,
	std::string&       out_loaderMessage)
{
	try
	{
		setValue(owner, static_cast<real>(std::stold(sdl)));
		return true;
	}
	catch(const std::exception& e)
	{
		out_loaderMessage += "exception on parsing real (" + e.what() + ")";
		out_loaderMessage += ", default to " + std::to_string(this->m_defaultValue);

		setValueToDefault(owner);

		return false;
	}
	catch(...)
	{
		out_loaderMessage += "unknown exception occurred on parsing real";
		out_loaderMessage += ", default to " + std::to_string(this->m_defaultValue);

		setValueToDefault(owner);

		return false;
	}
}

template<typename Owner, typename RealType>
inline void TSdlReal<Owner, RealType>::convertToSdl(
	Owner&       owner,
	std::string* out_sdl,
	std::string& out_converterMessage) const
{
	PH_ASSERT(out_sdl);

	// TODO
	PH_ASSERT_UNREACHABLE_SECTION();
}

}// end namespace ph
