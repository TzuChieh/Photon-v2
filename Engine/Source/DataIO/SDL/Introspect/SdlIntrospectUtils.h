#pragma once

#include "Common/primitive_type.h"
#include "Common/assertion.h"

#include <optional>
#include <type_traits>

namespace ph
{

class SdlIntrospectUtils final
{
public:
	template<typename Value>
	std::optional<Value> loadValue(const std::string& sdlValue, std::string* out_loaderMsg = nullptr);

	std::optional<real> loadReal(const std::string& sdlValue, std::string* out_loaderMsg = nullptr);
	std::optional<integer> loadInteger(const std::string& sdlValue, std::string* out_loaderMsg = nullptr);
};

// Implementations:

template<typename Value>
std::optional<Value> SdlIntrospectUtils::loadValue(const std::string& sdlValue, std::string* const out_loaderMsg)
{
	if constexpr(std::is_same_v<Value, real>)
	{
		return loadReal(sdlValue, out_loaderMsg);
	}
	else if constexpr(std::is_same_v<Value, integer>)
	{
		return loadInteger(sdlValue, out_loaderMsg);
	}
	else
	{
		if(out_loaderMsg)
		{
			*out_loaderMsg = 
		}

		return std::nullopt;
	}
}

}// end namespace ph
