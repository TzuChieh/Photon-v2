#include "FileIO/DescriptionParser.h"
#include "FileIO/Description.h"
#include "FileIO/RenderOption.h"
#include "FileIO/SDL/TCommandInterface.h"
#include "FileIO/SDL/CommandEntry.h"
#include "FileIO/SDL/SdlTypeInfo.h"

#include <iostream>
#include <sstream>

namespace ph
{

std::string DescriptionParser::CORE_DATA_NAME()
{
	return "@__sdl_core_name";
}

bool DescriptionParser::addCommandEntry(const CommandEntry& entry)
{
	if(!entry.typeInfo().isValid())
	{
		std::cerr << "warning: command entry has invalid type info: " 
		          << entry.typeInfo().toString()
		          << ", not adding" << std::endl;
		return false;
	}

	const std::string& fullTypeName = getFullTypeName(entry.typeInfo());
	const auto&        iter         = NAMED_INTERFACE_MAP().find(fullTypeName);
	if(iter != NAMED_INTERFACE_MAP().end())
	{
		std::cerr << "warning: command entry of type <" 
		          << entry.typeInfo().toString() 
		          << "> is already present, not adding" << std::endl;
		return false;
	}

	NAMED_INTERFACE_MAP()[fullTypeName] = entry;
	return true;
}

CommandEntry DescriptionParser::getCommandEntry(const SdlTypeInfo& typeInfo)
{
	const std::string& fullTypeName = getFullTypeName(typeInfo);
	const auto&        iter         = NAMED_INTERFACE_MAP().find(fullTypeName);
	if(iter == NAMED_INTERFACE_MAP().end())
	{
		std::cerr << "warning: command entry <" 
		          << fullTypeName 
		          << "> does not exist" << std::endl;
		return CommandEntry();
	}

	return NAMED_INTERFACE_MAP()[fullTypeName];
}

std::unordered_map<std::string, CommandEntry>& DescriptionParser::NAMED_INTERFACE_MAP()
{
	static std::unordered_map<std::string, CommandEntry> namedInterfaceMap;
	return namedInterfaceMap;
}

DescriptionParser::DescriptionParser() : 
	m_commandCache(), 
	m_coreCommandTokenizer ({' ', '\t', '\n', '\r'}, {{'(',  ')'}, {'[', ']'}}),
	m_worldCommandTokenizer({' ', '\t', '\n', '\r'}, {{'\"', '\"'}, {'[', ']'}, {'(', ')'}}), 
	m_nameTokenizer        ({},                      {{'\"', '\"'}}),// TODO: there may be a need to skip spaces, e.g., fun(    "@ some name")
	m_generatedNameCounter(0),
	m_workingDirectory()
{}

void DescriptionParser::enter(const std::string& commandFragment, Description& out_data)
{
	if(getCommandType(commandFragment) != ECommandType::UNKNOWN)
	{
		parseCommand(m_commandCache, out_data);
		m_commandCache.clear();
		m_commandCache.shrink_to_fit();
	}

	m_commandCache += commandFragment;
}

void DescriptionParser::parseCommand(const std::string& command, Description& out_data)
{
	if(command.empty())
	{
		return;
	}

	const ECommandType commandType = getCommandType(command);

	if(commandType == ECommandType::WORLD)
	{
		parseWorldCommand(command, out_data);
	}
	else if(commandType == ECommandType::CORE)
	{
		parseCoreCommand(command, out_data);
	}
	else if(commandType == ECommandType::COMMENT)
	{
		// do nothing
	}
	else
	{
		std::cerr << "warning: at DescriptionParser::parseCachedCommand(), "
		          << "unsupported command type detected" << std::endl;
	}
}

void DescriptionParser::parseCoreCommand(const std::string& command, Description& out_data)
{
	auto& resources = out_data.resources;

	std::vector<std::string> tokens;
	m_coreCommandTokenizer.tokenize(command, tokens);

	// skip command-prefix-only command
	if(tokens.size() == 1)
	{
		return;
	}

	if(tokens.size() < 3)
	{
		std::cerr << "warning: at DescriptionParser::parseCoreCommand(), "
		          << "bad formatted command <" + command + ">" << std::endl;
		return;
	}

	const std::string&             categoryName = tokens[1];
	const std::string&             typeName     = tokens[2];
	const SdlTypeInfo              typeInfo(SdlTypeInfo::nameToCategory(categoryName), typeName);
	const std::vector<std::string> clauseStrings(tokens.begin() + 3, tokens.end());
	const InputPacket              inputPacket(getValueClauses(clauseStrings), &out_data.resources, m_workingDirectory);
	const SdlLoader&               loader = getCommandEntry(typeInfo).getLoader();

	auto loadedResource = loader.load(inputPacket);
	out_data.resources.addResource(typeInfo, CORE_DATA_NAME(), std::move(loadedResource));
}

void DescriptionParser::parseWorldCommand(const std::string& command, Description& out_data)
{
	std::vector<std::string> tokens;
	m_worldCommandTokenizer.tokenize(command, tokens);

	// skip command-prefix-only command
	if(tokens.size() == 1)
	{
		return;
	}

	if(isLoadCommand(tokens))
	{
		const std::string&             categoryName = tokens[1];
		const std::string&             typeName     = tokens[2];
		const std::string&             resourceName = getName(tokens[3]);
		const SdlTypeInfo              typeInfo(SdlTypeInfo::nameToCategory(categoryName), typeName);
		const std::vector<std::string> clauseStrings(tokens.begin() + 4, tokens.end());
		const InputPacket              inputPacket(getValueClauses(clauseStrings), &out_data.resources, m_workingDirectory);
		const SdlLoader&               loader = getCommandEntry(typeInfo).getLoader();

		auto loadedResource = loader.load(inputPacket);
		out_data.resources.addResource(typeInfo, resourceName, std::move(loadedResource));
	}
	else if(isExecuteCommand(tokens))
	{
		const std::string&             categoryName = tokens[1];
		const std::string&             typeName     = tokens[2];
		const std::string&             executorName = tokens[3];
		const std::string&             targetResourceName = getName(tokens[4]);
		const SdlTypeInfo              ownerTypeInfo(SdlTypeInfo::nameToCategory(categoryName), typeName);
		const std::vector<std::string> clauseStrings(tokens.begin() + 5, tokens.end());
		const InputPacket              inputPacket(getValueClauses(clauseStrings), &out_data.resources, m_workingDirectory);
		
		const auto& commandEntry   = getCommandEntry(ownerTypeInfo);
		const auto& executor       = commandEntry.getExecutor(executorName);
		const auto& targetTypeInfo = executor.getTargetTypeInfo();

		const DataTreatment targetResourceDT = targetResourceName.empty() ?
			DataTreatment::OPTIONAL() :
			DataTreatment::REQUIRED("cannot find specified target resource");

		const auto& targetResource = out_data.resources.getResource(targetTypeInfo, 
		                                                            targetResourceName, 
		                                                            targetResourceDT);
		
		//ExitStatus status = commandEntry.execute(targetResource, executorName, inputPacket);
		const ExitStatus& status = executor.execute(targetResource, inputPacket);

		const std::string& funcInfo = "type <" + ownerTypeInfo.toString() + ">'s executor: " +
			executor.toString();

		switch(status.state)
		{
		case ExitStatus::State::SUCCESS:
			if(!status.message.empty())
			{
				std::cout << funcInfo << " successfully executed" << std::endl;
				std::cout << status.message << std::endl;
			}
			break;

		case ExitStatus::State::WARNING:
			std::cerr << funcInfo << " executed, but with warning" << std::endl;
			std::cerr << status.message << std::endl;
			break;

		case ExitStatus::State::FAILURE:
			std::cerr << funcInfo << " executed and failed" << std::endl;
			std::cerr << status.message << std::endl;
			break;

		case ExitStatus::State::BAD_INPUT:
			std::cerr << funcInfo << " ignored because of bad input" << std::endl;
			std::cerr << status.message << std::endl;
			break;

		case ExitStatus::State::UNSUPPORTED:
			std::cerr << "calling unsupported function: " << funcInfo << std::endl;
			if(!status.message.empty())
			{
				std::cerr << status.message << std::endl;
			}
			break;
		}
	}
	else
	{
		std::cerr << "warning: at DescriptionParser::parseWorldCommand(), "
		          << "unknown command <" + command + ">" << std::endl;
		return;
	}
}

std::string DescriptionParser::genName()
{
	return "@__item-" + std::to_string(m_generatedNameCounter++);
}

std::string DescriptionParser::getName(const std::string& nameToken) const
{
	std::vector<std::string> tokens;
	m_nameTokenizer.tokenize(nameToken, tokens);
	if(tokens.size() != 1)
	{
		return "";
	}
	else if(tokens[0].front() != '@' || tokens[0].length() == 1)
	{
		// possibly using genName() if anonymous declaration is supported
		return "";
	}
	else
	{
		return tokens[0];
	}
}

void DescriptionParser::setWorkingDirectory(const Path& path)
{
	m_workingDirectory = path;
}

std::vector<ValueClause> DescriptionParser::getValueClauses(const std::vector<std::string>& clauseStrings)
{
	std::vector<ValueClause> vClauses;
	for(const auto& clauseString : clauseStrings)
	{
		vClauses.push_back(ValueClause(clauseString));
	}
	return vClauses;
}

ECommandType DescriptionParser::getCommandType(const std::string& command)
{
	if(command.compare(0, 2, "->") == 0)
	{
		return ECommandType::WORLD;
	}
	else if(command.compare(0, 2, "##") == 0)
	{
		return ECommandType::CORE;
	}
	else if(command.compare(0, 2, "//") == 0)
	{
		return ECommandType::COMMENT;
	}
	else
	{
		return ECommandType::UNKNOWN;
	}
}

bool DescriptionParser::isResourceName(const std::string& token) const
{
	return !getName(token).empty();
}

std::string DescriptionParser::getFullTypeName(const SdlTypeInfo& typeInfo)
{
	const std::string& categoryName = typeInfo.getCategoryName();
	const std::string& typeName     = typeInfo.typeName;
	return categoryName + '_' + typeName;
}

bool DescriptionParser::isLoadCommand(const std::vector<std::string>& commandTokens) const
{
	if(commandTokens.size() >= 4)
	{
		if(isResourceName(commandTokens[3]))
		{
			return true;
		}
	}

	return false;
}

bool DescriptionParser::isExecuteCommand(const std::vector<std::string>& commandTokens) const
{
	if(commandTokens.size() >= 5)
	{
		if(isResourceName(commandTokens[4]))
		{
			return true;
		}
	}

	return false;
}

}// end namespace ph