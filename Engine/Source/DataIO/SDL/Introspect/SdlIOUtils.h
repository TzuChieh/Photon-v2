#pragma once

#include "Common/primitive_type.h"
#include "Common/assertion.h"
#include "Math/TVector3.h"

#include <optional>

namespace ph
{

class SdlIOUtils final
{
public:
	static std::optional<real> loadReal(const std::string& sdlReal, std::string* out_loaderMsg = nullptr);
	static std::optional<integer> loadInteger(const std::string& sdlInteger, std::string* out_loaderMsg = nullptr);
	static std::optional<math::Vector3R> loadVector3R(const std::string& sdlVector3R, std::string* out_loaderMsg = nullptr);
};

// Implementations:

}// end namespace ph
