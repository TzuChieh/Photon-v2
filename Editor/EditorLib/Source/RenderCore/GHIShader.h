#pragma once

#include "RenderCore/ghi_enums.h"

#include "Common/primitive_type.h"

#include <string>
#include <string_view>
#include <variant>

namespace ph::editor
{

class GHIShader
{
public:
	using NativeHandle = std::variant<
		std::monostate,
		uint64>;

	GHIShader(std::string name, EGHIInfoShadingStage shadingStage);
	virtual ~GHIShader();

	virtual NativeHandle getNativeHandle();

	std::string_view getName() const;
	EGHIInfoShadingStage getShadingStage() const;

private:
	std::string m_name;
	EGHIInfoShadingStage m_shadingStage;
};

inline auto GHIShader::getNativeHandle()
-> NativeHandle
{
	return std::monostate();
}

inline std::string_view GHIShader::getName() const
{
	return m_name;
}

inline EGHIInfoShadingStage GHIShader::getShadingStage() const
{
	return m_shadingStage;
}

}// end namespace ph::editor
