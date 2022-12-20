#pragma once

#include "ThirdParty/glad2.h"

#include <Utility/IUninstantiable.h>
#include <Utility/utility.h>

#include <concepts>

namespace ph::editor
{

class Opengl final : private IUninstantiable
{
public:
	static GLboolean getBooleanv(GLenum pname);

	bool getBoolean(GLenum pname);

	static GLfloat getFloatv(GLenum pname);

	template<std::floating_point T>
	static T getFloat(GLenum pname);

	static GLint getIntegerv(GLenum pname);

	template<std::integral T>
	static T getInteger(GLenum pname);
};

inline bool Opengl::getBoolean(const GLenum pname)
{
	return getBooleanv(pname) == GL_TRUE;
}

template<std::floating_point T>
T Opengl::getFloat(const GLenum pname)
{
	return getFloatv(pname);
}

}// end namespace ph::editor
