#pragma once

#include "RenderCore/ghi_enums.h"

#include "Common/primitive_type.h"

#include <string>
#include <string_view>
#include <variant>
#include <memory>

namespace ph::editor::ghi
{

class Shader
{
public:
	using NativeHandle = std::variant<
		std::monostate,
		uint64>;

	Shader(std::string name, EShadingStage shadingStage);
	virtual ~Shader();

	virtual NativeHandle getNativeHandle();

	std::string_view getName() const;
	EShadingStage getShadingStage() const;

private:
	std::string m_name;
	EShadingStage m_shadingStage;
};

inline auto Shader::getNativeHandle()
-> NativeHandle
{
	return std::monostate();
}

inline std::string_view Shader::getName() const
{
	return m_name;
}

inline EShadingStage Shader::getShadingStage() const
{
	return m_shadingStage;
}

}// end namespace ph::editor::ghi
