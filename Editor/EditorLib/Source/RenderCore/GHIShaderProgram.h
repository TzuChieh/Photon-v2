#pragma once

#include <Common/primitive_type.h>
#include <Common/assertion.h>
#include <Math/math_fwd.h>

#include <string>
#include <string_view>
#include <type_traits>

namespace ph::editor
{

class GHIShaderProgram
{
public:
	explicit GHIShaderProgram(std::string name);
	virtual ~GHIShaderProgram();

	virtual void bind() = 0;

	virtual void setInt32(std::string_view name, int32 value) = 0;
	virtual void setFloat32(std::string_view name, float32 value) = 0;
	virtual void setVector3F(std::string_view name, const math::Vector3F& value) = 0;
	virtual void setVector4F(std::string_view name, const math::Vector4F& value) = 0;
	virtual void setMatrix4F(std::string_view name, const math::Matrix4F& value) = 0;

	std::string_view getName() const;

	template<typename T>
	void setUniform(std::string_view name, const T& value);

private:
	std::string m_name;
};

inline std::string_view GHIShaderProgram::getName() const
{
	return m_name;
}

template<typename T>
inline void GHIShaderProgram::setUniform(std::string_view name, const T& value)
{
	if constexpr(std::is_same_v<T, int32>)
	{
		setInt32(name, value);
	}
	else if constexpr(std::is_same_v<T, float32>)
	{
		setFloat32(name, value);
	}
	else if constexpr(std::is_same_v<T, math::Vector3F>)
	{
		setVector3F(name, value);
	}
	else if constexpr(std::is_same_v<T, math::Vector4F>)
	{
		setVector4F(name, value);
	}
	else if constexpr(std::is_same_v<T, math::Matrix4F>)
	{
		setMatrix4F(name, value);
	}
	else
	{
		PH_ASSERT_UNREACHABLE_SECTION();
	}
}

}// end namespace ph::editor
