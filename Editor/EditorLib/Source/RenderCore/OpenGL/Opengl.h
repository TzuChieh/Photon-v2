#pragma once

#include "ThirdParty/glad2.h"

#include <Common/utility.h>
#include <Utility/IUninstantiable.h>

#include <concepts>

namespace ph::editor::ghi
{

class Opengl final : private IUninstantiable
{
public:
	/*! @brief `glGetBooleanv()` helpers.
	*/
	///@{
	static GLboolean getBooleanv(GLenum pname);

	bool getBoolean(GLenum pname);
	///@}

	/*! @brief `glGetFloatv()` helpers.
	*/
	///@{
	static GLfloat getFloatv(GLenum pname);

	template<std::floating_point T>
	static T getFloat(GLenum pname);
	///@}

	/*! @brief `glGetIntegerv()` helpers.
	*/
	///@{
	static GLint getIntegerv(GLenum pname);

	template<std::integral T>
	static T getInteger(GLenum pname);
	///@}
};

inline bool Opengl::getBoolean(const GLenum pname)
{
	return getBooleanv(pname) == GL_TRUE;
}

template<std::floating_point T>
T Opengl::getFloat(const GLenum pname)
{
	return lossless_float_cast<T>(getFloatv(pname));
}

template<std::integral T>
T Opengl::getInteger(const GLenum pname)
{
	return lossless_integer_cast<T>(getIntegerv(pname));
}

}// end namespace ph::editor::ghi
