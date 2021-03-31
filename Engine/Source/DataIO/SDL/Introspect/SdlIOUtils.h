#pragma once

#include "Common/primitive_type.h"
#include "Common/assertion.h"
#include "Math/TVector3.h"
#include "Math/TQuaternion.h"

#include <optional>
#include <vector>

namespace ph
{

class SdlIOUtils final
{
public:
	static std::optional<real> loadReal(
		const std::string& sdlRealStr, std::string* out_loaderMsg = nullptr);

	static std::optional<integer> loadInteger(
		const std::string& sdlIntegerStr, std::string* out_loaderMsg = nullptr);

	static std::optional<math::Vector3R> loadVector3R(
		const std::string& sdlVector3Str, std::string* out_loaderMsg = nullptr);

	static std::optional<math::QuaternionR> loadQuaternionR(
		const std::string& sdlQuaternionStr, std::string* out_loaderMsg = nullptr);

	static std::optional<std::vector<real>> loadRealArray(
		const std::string& sdlRealArrayStr, std::string* out_loaderMsg = nullptr);
};

// Implementations:

}// end namespace ph
