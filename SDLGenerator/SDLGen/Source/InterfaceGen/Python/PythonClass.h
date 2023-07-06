#pragma once

#include "InterfaceGen/Python/PythonMethod.h"

#include <string>
#include <vector>
#include <string_view>

namespace ph::sdlgen
{

class PythonClass final
{
public:
	inline PythonClass() = default;
	explicit PythonClass(std::string className);

	void addMethod(PythonMethod method);
	void addDefaultInit();
	void setInheritedClass(std::string inheritedClassName);
	bool hasMethod(std::string_view methodName) const;
	bool isInheriting() const;
	std::string getClassName() const;
	std::string getInheritedClassName() const;
	std::string genCode() const;

private:
	std::string               m_className;
	std::string               m_inheritedClassName;
	std::vector<PythonMethod> m_methods;
};

}// end namespace ph::sdlgen
