#include "RenderCore/OpenGL/Opengl.h"

namespace ph::editor::ghi
{

GLboolean Opengl::getBooleanv(const GLenum pname)
{
	GLboolean boolValue = GL_FALSE;
	glGetBooleanv(pname, &boolValue);
	return boolValue;
}

GLfloat Opengl::getFloatv(const GLenum pname)
{
	GLfloat floatValue = 0.0f;
	glGetFloatv(pname, &floatValue);
	return floatValue;
}

GLint Opengl::getIntegerv(const GLenum pname)
{
	GLint intValue = 0;
	glGetIntegerv(pname, &intValue);
	return intValue;
}

}// end namespace ph::editor::ghi
