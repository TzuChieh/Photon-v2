#include "InterfaceGen/Python/PythonClass.h"

#include <Common/assertion.h>
#include <Common/logging.h>

#include <utility>
#include <format>

namespace ph::sdlgen
{

PH_DEFINE_INTERNAL_LOG_GROUP(PythonClass, SDLGen);

PythonClass::PythonClass(std::string className) :
	m_className         (std::move(className)),
	m_inheritedClassName(),
	m_methods           ()
{
	PH_ASSERT(!m_className.empty());
}

void PythonClass::addMethod(PythonMethod method)
{
	m_methods.push_back(std::move(method));
}

void PythonClass::addDefaultInit()
{
	PythonMethod method("__init__");
	method.addCodeLine("super().__init__()");
	addMethod(std::move(method));
}

void PythonClass::setInheritedClass(std::string inheritedClassName)
{
	m_inheritedClassName = std::move(inheritedClassName);
}

bool PythonClass::hasMethod(const std::string_view methodName) const
{
	for(const PythonMethod& method : m_methods)
	{
		if(method.getMethodName() == methodName)
		{
			return true;
		}
	}

	return false;
}

std::string PythonClass::genCode() const
{
	std::string code;

	if(m_className.empty())
	{
		PH_LOG_WARNING(PythonClass, "no class name provided, ignoring");
		return "";
	}

	if(!m_inheritedClassName.empty())
	{
		code += std::format("class {}({}):\n",
			m_className, m_inheritedClassName);
	}
	else
	{
		code += std::format("class {}:\n", m_className);
	}

	if(!m_methods.empty())
	{
		for(const PythonMethod& method : m_methods)
		{
			code += method.genCode(1);
		}
	}
	else
	{
		code += std::format("{}pass\n\n",
			PythonMethod::UNIT_INDENT);
	}

	return code + "\n";
}

}// end namespace ph::sdlgen
