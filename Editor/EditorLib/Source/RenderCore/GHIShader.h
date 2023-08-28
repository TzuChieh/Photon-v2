#pragma once

#include "RenderCore/ghi_enums.h"

#include "Common/primitive_type.h"

#include <string>
#include <string_view>
#include <variant>
#include <memory>

namespace ph::editor
{

class GHIShader
{
public:
	using NativeHandle = std::variant<
		std::monostate,
		uint64>;

	GHIShader(std::string name, EGHIShadingStage shadingStage);
	virtual ~GHIShader();

	virtual NativeHandle getNativeHandle();

	std::string_view getName() const;
	EGHIShadingStage getShadingStage() const;

private:
	std::string m_name;
	EGHIShadingStage m_shadingStage;
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

inline EGHIShadingStage GHIShader::getShadingStage() const
{
	return m_shadingStage;
}

/*!
Basically stores a shader reference for each `EGHIShadingStage` entry.
*/
class GHIShaderSet final
{
public:
	std::shared_ptr<GHIShader> vertexShader;
	std::shared_ptr<GHIShader> fragmentShader;
	std::shared_ptr<GHIShader> computeShader;
};

}// end namespace ph::editor
