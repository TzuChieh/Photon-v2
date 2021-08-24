#include "InterfaceGen/Python/PythonMethod.h"

#include <Common/assertion.h>
#include <Common/logging.h>
#include <Utility/string_utils.h>

#include <utility>

namespace ph::sdlgen
{

PH_DEFINE_INTERNAL_LOG_GROUP(PythonMethod, SDLGen);

PythonMethod::PythonMethod(std::string methodName) :
	m_methodName(std::move(methodName)),
	m_codeLines (),
	m_inputs    ()
{
	PH_ASSERT(!m_methodName.empty());
}

void PythonMethod::addInput(
	std::string variableName,
	std::string defaultArgument,
	std::string expectedType)
{
	std::string inputString = std::move(variableName);

	if(!expectedType.empty())
	{
		inputString += ": " + expectedType;
	}

	if(!defaultArgument.empty())
	{
		inputString += " = " + defaultArgument;
	}

	m_inputs.push_back(std::move(inputString));
}

void PythonMethod::addCodeLine(std::string codeLine)
{
	m_codeLines.push_back(
		std::format("{}{}\n", UNIT_INDENT, std::move(codeLine)));
}

std::string PythonMethod::genCode(const std::size_t indentAmount) const
{
	std::string code;

	if(m_methodName.empty())
	{
		PH_LOG_WARNING(PythonMethod, "no method name provided, ignoring");
		return "";
	}

	std::string inputList;
	if(!m_inputs.empty())
	{
		inputList += m_inputs[0];
	}
	for(std::size_t i = 1; i < m_inputs.size(); ++i)
	{
		inputList += ", " + m_inputs[i];
	}

	const std::string indention = string_utils::repeat(UNIT_INDENT, indentAmount);

	code += std::format("{}def {}({}):\n", 
		indention, m_methodName, inputList);

	if(!m_codeLines.empty())
	{
		for(const std::string& codeLine : m_codeLines)
		{
			code += std::format("{}{}",
				indention, codeLine);
		}
	}
	else
	{
		code += std::format("{}{}pass\n",
			indention, UNIT_INDENT);
	}

	return code + "\n";
}

}// end namespace ph::sdlgen
