#include "DataIO/SDL/SdlParser.h"
#include "DataIO/SDL/SceneDescription.h"
#include "DataIO/SDL/TCommandInterface.h"
#include "DataIO/SDL/CommandEntry.h"
#include "DataIO/SDL/SdlTypeInfo.h"
#include "DataIO/SDL/InputPacket.h"
#include "Common/Logger.h"
#include "ph_cpp_core.h"
#include "DataIO/SDL/sdl_helpers.h"
#include "DataIO/SDL/ETypeCategory.h"
#include "Utility/string_utils.h"
#include "DataIO/SDL/Introspect/SdlClass.h"
#include "DataIO/SDL/sdl_exceptions.h"
#include "DataIO/SDL/Introspect/SdlInputContext.h"

#include <iostream>
#include <sstream>
#include <cstddef>

namespace ph
{

namespace
{

const Logger logger(LogSender("SDL Parser"));

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
	m_mangledNameToClass(),
	m_workingDirectory(),
	m_commandCache(),
	m_generatedNameCounter(0)
{
	const std::vector<const SdlClass*> sdlClasses = get_registered_sdl_classes();
	for(const SdlClass* const clazz : sdlClasses)
	{
		PH_ASSERT(clazz);

		const auto mangledClassName = getMangledName(clazz->genCategoryName(), clazz->getTypeName());

		const auto& iter = m_mangledNameToClass.find(mangledClassName);
		if(iter != m_mangledNameToClass.end())
		{
			logger.log(ELogLevel::FATAL_ERROR,
				"mangled SDL class name < " + 
				entry.typeInfo().toString() + 
				"> is already present, overwriting");
		}

		m_mangledNameToClass[mangledClassName] = clazz;
	}
}

void SdlParser::enter(const std::string_view commandSegment, SceneDescription& out_scene)
{
	const ESdlCommandType commandType = getCommandType(commandSegment);
	const bool hasEnteredNewCommand = commandType != ESdlCommandType::UNKNOWN;

	// If new command has been entered, parse cached commands first
	if(hasEnteredNewCommand)
	{
		flush(out_scene);
	}

	m_commandCache.append(commandSegment);
}

void SdlParser::flush(SceneDescription& out_scene)
{
	parseCommand(m_commandCache, out_scene);
	m_commandCache.clear();
	m_commandCache.shrink_to_fit();// TODO: reconsider, maybe only reset if too large
}

void SdlParser::parseCommand(const std::string& command, SceneDescription& out_scene)
{
	if(command.empty())
	{
		return;
	}

	const ESdlCommandType commandType = getCommandType(command);

	if(commandType == ESdlCommandType::LOAD)
	{
		parseLoadCommand(command, out_scene);
	}
	else if(commandType == ESdlCommandType::EXECUTION)
	{
		parseExecutionCommand(command, out_scene);
	}
	else if(commandType == ESdlCommandType::COMMENT)
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
	const ESdlCommandType type,
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
	else if(isExecutionCommand(tokens))
	{
		isParsed = parseExecutionCommand(type, tokens, out_scene);
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

	// TODO: log "resource ignored due to error -> " 

	return isParsed;
}

void SdlParser::parseLoadCommand(
	const std::string& command,
	SceneDescription&  out_scene)
{
	static const Tokenizer loadCommandTokenizer(
		{' ', '\t', '\n', '\r'}, 
		{{'\"', '\"'}, {'[', ']'}, {'(', ')'}});

	std::vector<std::string> tokens;
	loadCommandTokenizer.tokenize(command, tokens);

	PH_ASSERT(getCommandType(tokens[0]) == ESdlCommandType::LOAD);

	// Sanity check
	if(tokens.size() < 4)
	{
		throw SdlLoadError(
			"syntax error: improper load command detected");
	}

	// Get category and type then acquire the matching SDL class

	const std::string_view categoryName = tokens[1];
	const std::string_view typeName     = tokens[2];

	std::string mangledClassName;
	getMangledName(categoryName, typeName, &mangledClassName);

	const SdlClass* const clazz = getSdlClass(mangledClassName);
	if(!clazz)
	{
		throw SdlLoadError(
			"SDL class <" + std::string(categoryName) + ", " + std::string(typeName) + "> does not exist");
	}

	// Initialize SDL resource from input value clauses

	std::shared_ptr<ISdlResource> resource = clazz->createResource();
	if(!resource)
	{
		throw SdlLoadError(
			"SDL class <" + clazz->genPrettyName() + "> generates empty resource");
	}

	const std::string& resourceName = getName(tokens[3]);

	const auto& clauseStrings = std::vector<std::string>(tokens.begin() + 4, tokens.end());
	ValueClauses clauses;
	getClauses(clauseStrings, &clauses);

	SdlInputContext inputContext(&out_scene, m_workingDirectory, clazz);
	clazz->initResource(*resource, clauses, inputContext);

	// Finally, add the resource to storage

	out_scene.addResource(std::move(resource), resourceName);
}

void SdlParser::parseExecutionCommand(
	const std::string& command,
	SceneDescription&  out_scene)
{
	static const Tokenizer executionCommandTokenizer(
		{' ', '\t', '\n', '\r'}, 
		{{'\"', '\"'}, {'[', ']'}, {'(', ')'}});

	std::vector<std::string> tokens;
	executionCommandTokenizer.tokenize(command, tokens);

	PH_ASSERT(getCommandType(tokens[0]) == ESdlCommandType::EXECUTION);

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

bool SdlParser::cacheCommandSegment(const std::string_view commandSegment)
{
	if(getCommandType(commandSegment) != ECommandType::UNKNOWN)
	{
		parseCommand(m_commandCache, out_scene);
		m_commandCache.clear();
		m_commandCache.shrink_to_fit();// TODO: reconsider, maybe only reset if too large
	}

	m_commandCache += commandFragment;
}

std::string SdlParser::genName()
{
	return "@__item-" + std::to_string(m_generatedNameCounter++);
}

std::string SdlParser::getName(const std::string& nameToken) const
{
	static const Tokenizer nameTokenizer(
		{},
		{{'\"', '\"'}})// TODO: there may be a need to skip spaces, e.g., fun(    "@ some name")

	std::vector<std::string> tokens;
	nameTokenizer.tokenize(nameToken, tokens);
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

ESdlCommandType SdlParser::getCommandType(const std::string_view commandSegment)
{
	// Skips any leading whitespace
	const auto trimmedSegment = string_utils::trim_head(commandSegment);

	// We cannot know the type yet with too few characters
	if(trimmedSegment.size() < 2)
	{
		return ESdlCommandType::UNKNOWN;
	}
	
	// Test to see if any command symbol is matched
	//
	// "//": comment
	// "+>": load
	// "->": removal
	// "=>": execution

	PH_ASSERT_GE(trimmedSegment.size(), 2);
	switch(trimmedSegment[0])
	{
	case '/': return trimmedSegment[1] == '/' ? ESdlCommandType::COMMENT   : ESdlCommandType::UNKNOWN;
	case '+': return trimmedSegment[1] == '>' ? ESdlCommandType::LOAD      : ESdlCommandType::UNKNOWN;
	case '-': return trimmedSegment[1] == '>' ? ESdlCommandType::REMOVAL   : ESdlCommandType::UNKNOWN;
	case '=': return trimmedSegment[1] == '/' ? ESdlCommandType::EXECUTION : ESdlCommandType::UNKNOWN;
	}

	return ESdlCommandType::UNKNOWN;
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

std::string SdlParser::getMangledName(const std::string_view categoryName, const std::string_view typeName)
{
	std::string mangledName;
	getMangledName(categoryName, typeName, &mangledName);
	return mangledName;
}

void SdlParser::getMangledName(const std::string_view categoryName, const std::string_view typeName, std::string* const out_mangledName)
{
	PH_ASSERT(out_mangledName);

	out_mangledName->clear();
	*out_mangledName += std::string(categoryName) + std::string(typeName);
}

void SdlParser::getClauses(const std::vector<std::string>& clauseStrings, ValueClauses* const out_clauses)
{
	PH_ASSERT(out_clauses);

	out_clauses->clear();
	for(const auto& clauseString : clauseStrings)
	{
		ValueClauses::Clause clause;
		getClause(clauseString, &clause);
		out_clauses->add(std::move(clause));
	}
}

void SdlParser::getClause(const std::string_view clauseString, ValueClauses::Clause* const out_clause)
{
	PH_ASSERT(out_clause);

	static const Tokenizer clauseTokenizer({' ', '\t', '\n', '\r'}, {{'\"', '\"'}, {'{', '}'}});

	if(clauseString.empty())
	{
		throw SdlLoadError("syntax error: clause string is empty");
	}

	std::vector<std::string> tokens;
	clauseTokenizer.tokenize(clauseString, tokens);
	if(tokens.size() != 3)
	{
		throw SdlLoadError(
			"syntax error: bad number of tokens < " + tokens.size() + ">, expected to be 3");
	}

	out_clause->type  = tokens[0];
	out_clause->value = tokens[2];
	
	// Parse name and an optional tag
	// tokens[1] contains name and tag, syntax: <name>:<optional-tag>

	const std::string_view nameAndTag = tokens[1];
	const auto colonPos = nameAndTag.find(':');
	if(colonPos == std::string_view::npos)
	{
		out_clause->name = tokens[1];
		out_clause->tag  = "";
	}
	else
	{
		out_clause->name = std::string(nameAndTag.substr(0, colonPos + 1));
		out_clause->tag  = nameAndTag.substr(colonPos);
	}
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

bool SdlParser::isExecutionCommand(const std::vector<std::string>& commandTokens) const
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

const SdlClass* SdlParser::getSdlClass(const std::string& mangledClassName) const
{
	const auto& iter = m_mangledNameToClass.find(mangledClassName);
	return iter != m_mangledNameToClass.end() ? iter->second : nullptr;
}

}// end namespace ph
