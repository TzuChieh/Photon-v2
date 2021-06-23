#include "DataIO/SDL/SdlParser.h"
#include "DataIO/SDL/Tokenizer.h"
#include "DataIO/SDL/SceneDescription.h"
#include "Common/Logger.h"
#include "ph_cpp_core.h"
#include "DataIO/SDL/sdl_helpers.h"
#include "DataIO/SDL/ETypeCategory.h"
#include "Utility/string_utils.h"
#include "DataIO/SDL/Introspect/SdlClass.h"
#include "DataIO/SDL/sdl_exceptions.h"
#include "DataIO/SDL/Introspect/SdlInputContext.h"

#include <cstddef>

namespace ph
{

namespace
{

const Logger logger(LogSender("SDL Parser"));

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
			logger.log(ELogLevel::WARNING_MED,
				"SDL class < " + clazz->genPrettyName() + "> already registered, "
				"overwriting; please check for name collision: mangled name is <" + 
				mangledClassName + ">");
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

	const ESdlCommandType type = getCommandType(command);

	try
	{
		parseSingleCommand(type, command, out_scene);
	}
	catch(const SdlLoadError& e)
	{
		logger.log(ELogLevel::WARNING_MED,
			"command failed to run -> " + e.whatStr());
	}
}

void SdlParser::parseSingleCommand(const ESdlCommandType type, const std::string& command, SceneDescription& out_scene)
{
	switch(type)
	{
	case ESdlCommandType::LOAD:
		parseLoadCommand(command, out_scene);
		break;

	case ESdlCommandType::EXECUTION:
		parseExecutionCommand(command, out_scene);
		break;

	case ESdlCommandType::COMMENT:
		// do nothing
		break;

	case ESdlCommandType::DIRECTIVE:
		// TODO
		break;

	default:
		throw SdlLoadError("unsupported command type");
		break;
	}
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
			"syntax error: improper load command");
	}

	// Get category and type then acquire the matching SDL class

	const std::string_view categoryName = tokens[1];
	const std::string_view typeName     = tokens[2];

	const SdlClass& clazz = getSdlClass(categoryName, typeName);

	// Initialize SDL resource from input value clauses

	std::shared_ptr<ISdlResource> resource = clazz.createResource();
	if(!resource)
	{
		throw SdlLoadError(
			"SDL class <" + clazz.genPrettyName() + "> generates empty resource");
	}

	const std::string& resourceName = getName(tokens[3]);

	const auto& clauseStrings = std::vector<std::string>(tokens.begin() + 4, tokens.end());
	ValueClauses clauses;
	getClauses(clauseStrings, &clauses);

	SdlInputContext inputContext(&out_scene, m_workingDirectory, &clazz);
	clazz.initResource(*resource, clauses, inputContext);

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

	// Sanity check
	if(tokens.size() < 5)
	{
		throw SdlLoadError(
			"syntax error: improper execution command");
	}

	// Get category and type then acquire the matching SDL class

	const std::string_view categoryName = tokens[1];
	const std::string_view typeName     = tokens[2];

	const SdlClass& clazz = getSdlClass(categoryName, typeName);

	// Get target SDL resource and clauses

	const std::string& targetResourceName = getName(tokens[4]);
	const std::string& executorName       = tokens[3];

	std::shared_ptr<ISdlResource> resource = out_scene.getResource(targetResourceName, clazz.getCategory());
	if(!resource)
	{
		throw SdlLoadError(
			"execution command failed: cannot find target resource <" + 
			targetResourceName + "> for the execution of <" + executorName + "> "
			"(SDL class: " + clazz.genPrettyName() + ")");
	}

	const auto& clauseStrings = std::vector<std::string>(tokens.begin() + 5, tokens.end());
	ValueClauses clauses;
	getClauses(clauseStrings, &clauses);

	// Finally, call the executor

	SdlInputContext inputContext(&out_scene, m_workingDirectory, &clazz);
	clazz.call(executorName, resource.get(), clauses, inputContext);
}

std::string SdlParser::genNameForAnonymity()
{
	return "@__anonymous-item-" + std::to_string(m_generatedNameCounter++);
}

std::string SdlParser::getName(const std::string_view resourceNameToken)
{
	// Remove any leading and trailing blank characters
	const auto trimmedToken = string_utils::trim(resourceNameToken);

	// Should at least contain a '@' character
	if(trimmedToken.empty())
	{
		throw SdlLoadError(
			"syntax error: resource name is empty, <" + std::string(resourceNameToken) + "> was given");
	}

	switch(trimmedToken.front())
	{
	// Token is a name without quotes
	case '@':
		// Token has more character(s) following '@'
		if(trimmedToken.size() > 1)
		{
			return std::string(trimmedToken);
		}
		// Token is anonymous
		else
		{
			return genNameForAnonymity();
		}

	// Token is a name with quotes
	case '\"':
		// Should at least contain 3 characters: opening and closing double quotes, the name prefix '@'
		if(trimmedToken.size() >= 3 && trimmedToken.back() == '\"' && trimmedToken[1] == '@')
		{
			// Token has more character(s) following '@'
			if(trimmedToken.size() > 4)
			{
				// Remove the double quotes
				return std::string(trimmedToken.substr(1, trimmedToken.size() - 2));
			}
			// Token is anonymous
			else
			{
				return genNameForAnonymity();
			}
		}
		else
		{
			throw SdlLoadError(
				"syntax error: resource name missing ending double quote and/or the @ prefix, "
				"<" + std::string(resourceNameToken) + "> was given");
		}

	default:
		throw SdlLoadError(
			"syntax error: resource name should start with @, optionally "
			"enclosed by double quotes, <" + std::string(resourceNameToken) + "> was given");
	}
}

void SdlParser::setWorkingDirectory(const Path& path)
{
	m_workingDirectory = path;
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
	// "##": directive

	PH_ASSERT_GE(trimmedSegment.size(), 2);
	switch(trimmedSegment[0])
	{
	case '/': return trimmedSegment[1] == '/' ? ESdlCommandType::COMMENT   : ESdlCommandType::UNKNOWN;
	case '+': return trimmedSegment[1] == '>' ? ESdlCommandType::LOAD      : ESdlCommandType::UNKNOWN;
	case '-': return trimmedSegment[1] == '>' ? ESdlCommandType::REMOVAL   : ESdlCommandType::UNKNOWN;
	case '=': return trimmedSegment[1] == '/' ? ESdlCommandType::EXECUTION : ESdlCommandType::UNKNOWN;
	case '#': return trimmedSegment[1] == '#' ? ESdlCommandType::DIRECTIVE : ESdlCommandType::UNKNOWN;
	}

	return ESdlCommandType::UNKNOWN;
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

	static const Tokenizer clauseTokenizer(
		{' ', '\t', '\n', '\r'}, 
		{{'\"', '\"'}, {'{', '}'}});

	if(clauseString.empty())
	{
		throw SdlLoadError("syntax error: clause string is empty");
	}

	// TODO: tokenize string_view
	std::vector<std::string> tokens;
	clauseTokenizer.tokenize(std::string(clauseString), tokens);
	if(tokens.size() != 3)
	{
		throw SdlLoadError(
			"syntax error: bad number of tokens < " + std::to_string(tokens.size()) + ">, expected to be 3");
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

const SdlClass* SdlParser::getSdlClass(const std::string& mangledClassName) const
{
	const auto& iter = m_mangledNameToClass.find(mangledClassName);
	return iter != m_mangledNameToClass.end() ? iter->second : nullptr;
}

const SdlClass& SdlParser::getSdlClass(const std::string_view categoryName, const std::string_view typeName) const
{
	std::string mangledClassName;
	getMangledName(categoryName, typeName, &mangledClassName);

	const SdlClass* const clazz = getSdlClass(mangledClassName);
	if(!clazz)
	{
		throw SdlLoadError(
			"SDL class <" + std::string(categoryName) + ", " + std::string(typeName) + "> does not exist");
	}

	return *clazz;
}

}// end namespace ph
