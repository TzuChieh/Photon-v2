#include "DataIO/SDL/SdlParser.h"
#include "DataIO/SDL/SceneDescription.h"
#include "DataIO/SDL/TCommandInterface.h"
#include "DataIO/SDL/CommandEntry.h"
#include "DataIO/SDL/SdlTypeInfo.h"
#include "DataIO/SDL/InputPacket.h"
#include "Common/Logger.h"

#include <iostream>
#include <sstream>

namespace ph
{

namespace
{
	Logger logger(LogSender("SDL Parser"));
}

bool SdlParser::addCommandEntry(const CommandEntry& entry)
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

	/*logger.log(ELogLevel::NOTE_MIN,
		"entry added: " + entry.typeInfo().toString());*/

	return true;
}

CommandEntry SdlParser::getCommandEntry(const SdlTypeInfo& typeInfo)
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

std::unordered_map<std::string, CommandEntry>& SdlParser::NAMED_INTERFACE_MAP()
{
	static std::unordered_map<std::string, CommandEntry> namedInterfaceMap;
	return namedInterfaceMap;
}

SdlParser::SdlParser() :
	m_commandCache(),

	m_regularCommandTokenizer(
		{' ', '\t', '\n', '\r'}, 
		{{'\"', '\"'}, {'[', ']'}, {'(', ')'}}),

	m_nameTokenizer(
		{},
		{{'\"', '\"'}}),// TODO: there may be a need to skip spaces, e.g., fun(    "@ some name")

	m_generatedNameCounter(0),
	m_workingDirectory()
{}

void SdlParser::enter(const std::string& commandFragment, SceneDescription& out_scene)
{
	if(getCommandType(commandFragment) != ECommandType::UNKNOWN)
	{
		parseCommand(m_commandCache, out_scene);
		m_commandCache.clear();
		m_commandCache.shrink_to_fit();
	}

	m_commandCache += commandFragment;
}

void SdlParser::parseCommand(const std::string& command, SceneDescription& out_scene)
{
	if(command.empty())
	{
		return;
	}

	const ECommandType commandType = getCommandType(command);

	if(commandType == ECommandType::REGULAR)
	{
		parseRegularCommand(commandType, command, out_scene);
	}
	else if(commandType == ECommandType::COMMENT)
	{
		// do nothing
	}
	else
	{
		logger.log(ELogLevel::WARNING_MED, 
			"unsupported command type detected");
	}
}

bool SdlParser::parseRegularCommand(
	const ECommandType type,
	const std::string& command,
	SceneDescription&  out_scene)
{
	std::vector<std::string> tokens;
	m_regularCommandTokenizer.tokenize(command, tokens);

	// Skip command-prefix-only or empty command
	if(tokens.size() <= 1)
	{
		return true;
	}

	bool isParsed = false;
	if(isLoadCommand(tokens))
	{
		isParsed = parseLoadCommand(type, tokens, out_scene);
	}
	else if(isExecuteCommand(tokens))
	{
		isParsed = parseExecuteCommand(type, tokens, out_scene);
	}
	else
	{
		logger.log(ELogLevel::WARNING_MED,
			"unknown command detected");
		
		isParsed = false;
	}

	if(!isParsed)
	{
		logger.log(ELogLevel::WARNING_MED,
			"command not parsed due to error: <" + command + ">");
	}

	return isParsed;
}

bool SdlParser::parseLoadCommand(
	const ECommandType              type,
	const std::vector<std::string>& tokens,
	SceneDescription&               out_scene)
{
	// Sanity check
	if(tokens.size() < 4)
	{
		logger.log(ELogLevel::WARNING_MED,
			"badly formatted command detected");

		return false;
	}

	const std::string& categoryName  = tokens[1];
	const std::string& typeName      = tokens[2];
	const auto&        typeInfo      = SdlTypeInfo(SdlTypeInfo::nameToCategory(categoryName), typeName);
	const std::string& resourceName  = getName(tokens[3]);
	const auto&        clauseStrings = std::vector<std::string>(tokens.begin() + 4, tokens.end());
	const auto&        inputPacket   = InputPacket(getValueClauses(clauseStrings), &out_scene, m_workingDirectory);
	const SdlLoader&   loader        = getCommandEntry(typeInfo).getLoader();

	auto loadedResource = loader.load(inputPacket);
	const bool isResourceValid = loadedResource != nullptr;

	out_scene.addResource(typeInfo, resourceName, std::move(loadedResource));

	return isResourceValid;
}

bool SdlParser::parseExecuteCommand(
	const ECommandType              type,
	const std::vector<std::string>& tokens,
	SceneDescription&               out_scene)
{
	const std::string& categoryName       = tokens[1];
	const std::string& typeName           = tokens[2];
	const std::string& executorName       = tokens[3];
	const std::string& targetResourceName = getName(tokens[4]);
	const auto&        ownerTypeInfo      = SdlTypeInfo(SdlTypeInfo::nameToCategory(categoryName), typeName);
	const auto&        clauseStrings      = std::vector<std::string>(tokens.begin() + 5, tokens.end());
	const auto&        inputPacket        = InputPacket(getValueClauses(clauseStrings), &out_scene, m_workingDirectory);
		
	const auto& commandEntry   = getCommandEntry(ownerTypeInfo);
	const auto& executor       = commandEntry.getExecutor(executorName);
	const auto& targetTypeInfo = executor.getTargetTypeInfo();

	const DataTreatment targetResourceDT = targetResourceName.empty() ?
		DataTreatment::OPTIONAL() :
		DataTreatment::REQUIRED("cannot find specified target resource");

	const auto& targetResource = out_scene.getResource(
		targetTypeInfo,
		targetResourceName,
		targetResourceDT);
	
	// TODO: check null res?
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
		return true;

	case ExitStatus::State::WARNING:
		std::cerr << funcInfo << " executed, but with warning" << std::endl;
		std::cerr << status.message << std::endl;
		return true;

	case ExitStatus::State::FAILURE:
		std::cerr << funcInfo << " executed and failed" << std::endl;
		std::cerr << status.message << std::endl;
		return false;

	case ExitStatus::State::BAD_INPUT:
		std::cerr << funcInfo << " ignored because of bad input" << std::endl;
		std::cerr << status.message << std::endl;
		return false;

	case ExitStatus::State::UNSUPPORTED:
		std::cerr << "calling unsupported function: " << funcInfo << std::endl;
		if(!status.message.empty())
		{
			std::cerr << status.message << std::endl;
		}
		return false;
	}

	return false;
}

std::string SdlParser::genName()
{
	return "@__item-" + std::to_string(m_generatedNameCounter++);
}

std::string SdlParser::getName(const std::string& nameToken) const
{
	std::vector<std::string> tokens;
	m_nameTokenizer.tokenize(nameToken, tokens);
	if(tokens.size() != 1)
	{
		return "";
	}
	else if(tokens[0].front() != '@' || tokens[0].length() == 1)
	{
		// Possibly using genName() if anonymous declaration is supported
		return "";
	}
	else
	{
		return tokens[0];
	}
}

void SdlParser::setWorkingDirectory(const Path& path)
{
	m_workingDirectory = path;
}

std::vector<ValueClause> SdlParser::getValueClauses(const std::vector<std::string>& clauseStrings)
{
	std::vector<ValueClause> vClauses;
	for(const auto& clauseString : clauseStrings)
	{
		vClauses.push_back(ValueClause(clauseString));
	}
	return vClauses;
}

ECommandType SdlParser::getCommandType(const std::string& command)
{
	if(command.compare(0, 2, "->") == 0)
	{
		return ECommandType::REGULAR;
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

bool SdlParser::isResourceName(const std::string& token) const
{
	return !getName(token).empty();
}

std::string SdlParser::getFullTypeName(const SdlTypeInfo& typeInfo)
{
	const std::string& categoryName = typeInfo.getCategoryName();
	const std::string& typeName     = typeInfo.typeName;
	return categoryName + '_' + typeName;
}

bool SdlParser::isLoadCommand(const std::vector<std::string>& commandTokens) const
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

bool SdlParser::isExecuteCommand(const std::vector<std::string>& commandTokens) const
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
