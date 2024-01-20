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
	PythonClass() = default;
	explicit PythonClass(std::string className);

	void addMethod(PythonMethod method);
	void addDefaultInit();
	void setInheritedClass(std::string inheritedClassName);
	void setDoc(std::string doc);
	bool hasMethod(std::string_view methodName) const;
	bool isInheriting() const;
	const std::string& getClassName() const;
	const std::string& getInheritedClassName() const;
	const std::string& getDoc() const;
	std::string genCode() const;

private:
	std::string               m_className;
	std::string               m_inheritedClassName;
	std::string               m_doc;
	std::vector<PythonMethod> m_methods;
};

}// end namespace ph::sdlgen
