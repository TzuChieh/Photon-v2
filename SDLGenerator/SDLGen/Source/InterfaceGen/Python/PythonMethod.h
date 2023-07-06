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
	inline PythonMethod() = default;
	explicit PythonMethod(std::string methodName);

	void addInput(
		std::string variableName, 
		std::string defaultArgument = "",
		std::string expectedType = "");

	void addCodeLine(std::string codeLine);

	template<typename... Args>
	void addCodeLine(std::string_view formatStr, Args&&... args);

	const std::string& getMethodName() const;
	std::string genCode(std::size_t indentAmount = 0) const;

private:
	std::string              m_methodName;
	std::vector<std::string> m_codeLines;
	std::vector<std::string> m_inputs;
};

// In-header Implementations:

inline const std::string& PythonMethod::getMethodName() const
{
	return m_methodName;
}

template<typename... Args>
inline void PythonMethod::addCodeLine(const std::string_view formatStr, Args&&... args)
{
	addCodeLine(
		std::vformat(formatStr, std::make_format_args(std::forward<Args>(args)...)));
}

}// end namespace ph::sdlgen
