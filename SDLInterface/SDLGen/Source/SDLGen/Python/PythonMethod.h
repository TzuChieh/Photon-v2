#pragma once

#include <string>
#include <vector>
#include <string_view>
#include <cstddef>
#include <format>

namespace ph::sdlgen
{

class PythonMethod final
{
public:
	// 4 spaces
	inline static constexpr std::string_view UNIT_INDENT = "    ";

public:
	PythonMethod() = default;
	explicit PythonMethod(std::string methodName);

	void setDoc(std::string doc);

	void addInput(
		std::string variableName, 
		std::string defaultArgument = "",
		std::string expectedType = "");

	void addCodeLine(std::string codeLine);

	template<typename... Args>
	void addCodeLine(std::format_string<Args...> strFormat, Args&&... args);

	void beginIndent();
	void endIndent();

	const std::string& getMethodName() const;
	const std::string& getDoc() const;

	/*!
	@param indentAmount Additional indent to apply when generating code.
	*/
	std::string genCode(std::size_t indentAmount = 0) const;

private:
	std::string              m_methodName;
	std::string              m_doc;
	std::vector<std::string> m_codeLines;
	std::vector<std::string> m_inputs;

	/*! Indent amount in the method body, e.g., first code line has indent = 0, 
	body of a conditional would have indent >= 1. */
	std::size_t m_currentIndentAmount;
};

// In-header Implementations:

inline const std::string& PythonMethod::getMethodName() const
{
	return m_methodName;
}

inline const std::string& PythonMethod::getDoc() const
{
	return m_doc;
}

template<typename... Args>
inline void PythonMethod::addCodeLine(const std::format_string<Args...> strFormat, Args&&... args)
{
	// Intentionally not forwarding to `std::make_format_args` due to P2905R2
	addCodeLine(
		std::vformat(strFormat.get(), std::make_format_args(args...)));
}

}// end namespace ph::sdlgen
