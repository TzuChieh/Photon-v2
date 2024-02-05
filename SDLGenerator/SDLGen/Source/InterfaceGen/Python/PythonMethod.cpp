#include "InterfaceGen/Python/PythonMethod.h"

#include <Common/assertion.h>
#include <Common/logging.h>
#include <Common/Utility/string_utils.h>

#include <utility>

namespace ph::sdlgen
{

PH_DEFINE_INTERNAL_LOG_GROUP(PythonMethod, SDLGen);

PythonMethod::PythonMethod(std::string methodName)
	: m_methodName(std::move(methodName))
	, m_codeLines()
	, m_inputs()
	, m_currentIndentAmount(0)
{
	PH_ASSERT(!m_methodName.empty());

	addInput("self");
}

void PythonMethod::setDoc(std::string doc)
{
	m_doc = doc;
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
	// Code line in a method should be indented too (hence indent amount +1)
	const std::string indention = string_utils::repeat(UNIT_INDENT, m_currentIndentAmount + 1);

	m_codeLines.push_back(
		std::format("{}{}\n", indention, std::move(codeLine)));
}

void PythonMethod::beginIndent()
{
	++m_currentIndentAmount;
}

void PythonMethod::endIndent()
{
	PH_ASSERT_GT(m_currentIndentAmount, 0);

	--m_currentIndentAmount;
}

std::string PythonMethod::genCode(const std::size_t indentAmount) const
{
	std::string code;

	if(m_methodName.empty())
	{
		PH_LOG(PythonMethod, Warning, "no method name provided, ignoring");
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

	if(!m_doc.empty())
	{
		code += std::format("{}{}\"\"\"\n{}{}{}\n{}{}\"\"\"\n",
			indention, UNIT_INDENT, indention, UNIT_INDENT, m_doc, indention, UNIT_INDENT);
	}

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
