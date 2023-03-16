#include "DataIO/SDL/SdlParser.h"
#include "DataIO/SDL/Tokenizer.h"
#include "DataIO/SDL/SceneDescription.h"
#include "Common/logging.h"
#include "ph_cpp_core.h"
#include "DataIO/SDL/sdl_helpers.h"
#include "DataIO/SDL/ETypeCategory.h"
#include "Utility/string_utils.h"
#include "DataIO/SDL/Introspect/SdlClass.h"
#include "DataIO/SDL/sdl_exceptions.h"
#include "DataIO/SDL/Introspect/SdlInputContext.h"
#include "Common/stats.h"

#include <cstddef>

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(SdlParser, SDL);
PH_DEFINE_INTERNAL_TIMER_STAT(ParseCommandTotal, SDLParser);
PH_DEFINE_INTERNAL_TIMER_STAT(ParseLoadCommand, SDLParser);
PH_DEFINE_INTERNAL_TIMER_STAT(ParseExecutionCommand, SDLParser);
PH_DEFINE_INTERNAL_TIMER_STAT(ParseDirectiveCommand, SDLParser);
PH_DEFINE_INTERNAL_TIMER_STAT(GetCommandType, SDLParser);
PH_DEFINE_INTERNAL_TIMER_STAT(GetName, SDLParser);
PH_DEFINE_INTERNAL_TIMER_STAT(GetClauses, SDLParser);
PH_DEFINE_INTERNAL_TIMER_STAT(GetSDLClass, SDLParser);

SdlParser::SdlParser() :
	m_commandVersion      (),
	m_mangledNameToClass  (),
	m_workingDirectory    (),
	m_commandCache        (),
	m_generatedNameCounter(0),
	m_numParsedCommands   (0),
	m_numParseErrors      (0)
{
	const std::vector<const SdlClass*> sdlClasses = get_registered_sdl_classes();
	for(const SdlClass* const clazz : sdlClasses)
	{
		PH_ASSERT(clazz);

		const auto mangledClassName = getMangledName(clazz->genCategoryName(), clazz->getTypeName());

		const auto& iter = m_mangledNameToClass.find(mangledClassName);
		if(iter != m_mangledNameToClass.end())
		{
			PH_LOG_WARNING(SdlParser, 
				"SDL class <{}> already registered, overwriting; please check for name "
				"collision: mangled name is <{}>", 
				clazz->genPrettyName(), mangledClassName);
		}

		m_mangledNameToClass[mangledClassName] = clazz;
	}
}

void SdlParser::enter(const std::string_view commandSegment, SceneDescription& out_scene)
{
	const ESdlCommandType commandType = getCommandType(commandSegment);
	const bool hasEnteredNewCommand = commandType != ESdlCommandType::Unknown;

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
	PH_SCOPED_TIMER(ParseCommandTotal);

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
		PH_LOG_WARNING(SdlParser, "command failed to run -> {}", e.whatStr());

		++m_numParseErrors;
	}
}

void SdlParser::parseSingleCommand(const ESdlCommandType type, const std::string& command, SceneDescription& out_scene)
{
	switch(type)
	{
	case ESdlCommandType::Load:
		parseLoadCommand(command, out_scene);
		break;

	case ESdlCommandType::Execution:
		parseExecutionCommand(command, out_scene);
		break;

	case ESdlCommandType::Comment:
		// do nothing
		break;

	case ESdlCommandType::Directive:
		parseDirectiveCommand(command, out_scene);
		break;

	default:
		throw SdlLoadError("unsupported command type");
		break;
	}

	++m_numParsedCommands;
}

void SdlParser::parseLoadCommand(
	const std::string& command,
	SceneDescription& out_scene)
{
	static const Tokenizer loadCommandTokenizer(
		{' ', '\t', '\n', '\r'}, 
		{{'\"', '\"'}, {'[', ']'}, {'(', ')'}});

	PH_SCOPED_TIMER(ParseLoadCommand);

	std::vector<std::string> tokens;
	loadCommandTokenizer.tokenize(command, tokens);

	// Sanity check
	if(tokens.size() < 4)
	{
		throw SdlLoadError(
			"syntax error: improper load command");
	}

	PH_ASSERT(getCommandType(tokens[0]) == ESdlCommandType::Load);
	PH_ASSERT_GE(tokens.size(), 4);

	// Get category and type then acquire the matching SDL class

	const std::string_view categoryName = tokens[1];
	const std::string_view typeName     = tokens[2];

	const SdlClass& clazz = getSdlClass(categoryName, typeName);

	// Initialize SDL resource from input value clauses

	const std::string& resourceName = getName(tokens[3]);

	// Now we have name-related information, which is useful for debugging. 
	// Catch load errors here to provide name information and re-throw.
	try
	{
		std::shared_ptr<ISdlResource> resource = clazz.createResource();
		if(!resource)
		{
			throw SdlLoadError("empty resource generated");
		}

		const auto& clauseStrings = std::vector<std::string>(tokens.begin() + 4, tokens.end());
		ValueClauses clauses;
		getClauses(clauseStrings, &clauses);

		SdlInputContext inputContext(&out_scene, m_workingDirectory, &clazz);
		clazz.initResource(*resource, clauses, inputContext);

		// Finally, add the resource to storage

		out_scene.addResource(std::move(resource), resourceName);
	}
	catch(const SdlLoadError& e)
	{
		throw SdlLoadError(
			"failed to load resource <" + resourceName + "> "
			"(from SDL class: " + clazz.genPrettyName() + ") "
			"-> " + e.whatStr());
	}
}

void SdlParser::parseExecutionCommand(
	const std::string& command,
	SceneDescription& out_scene)
{
	static const Tokenizer executionCommandTokenizer(
		{' ', '\t', '\n', '\r'}, 
		{{'\"', '\"'}, {'[', ']'}, {'(', ')'}});

	PH_SCOPED_TIMER(ParseExecutionCommand);

	std::vector<std::string> tokens;
	executionCommandTokenizer.tokenize(command, tokens);

	// Sanity check
	if(tokens.size() < 5)
	{
		throw SdlLoadError(
			"syntax error: improper execution command");
	}

	PH_ASSERT(getCommandType(tokens[0]) == ESdlCommandType::Execution);
	PH_ASSERT_GE(tokens.size(), 5);

	// Get category and type then acquire the matching SDL class

	const std::string_view categoryName = tokens[1];
	const std::string_view typeName     = tokens[2];

	const SdlClass& clazz = getSdlClass(categoryName, typeName);

	// Get target SDL resource and clauses

	const std::string& targetResourceName = getName(tokens[4]);
	const std::string& executorName       = tokens[3];

	// Now we have name-related information, which is useful for debugging. 
	// Catch load errors here to provide name information and re-throw.
	try
	{
		std::shared_ptr<ISdlResource> resource = out_scene.getResource(targetResourceName, clazz.getCategory());
		if(!resource)
		{
			throw SdlLoadError("cannot find target resource from scene");
		}

		const auto& clauseStrings = std::vector<std::string>(tokens.begin() + 5, tokens.end());
		ValueClauses clauses;
		getClauses(clauseStrings, &clauses);

		// Finally, call the executor

		SdlInputContext inputContext(&out_scene, m_workingDirectory, &clazz);
		clazz.call(executorName, resource.get(), clauses, inputContext);
	}
	catch(const SdlLoadError& e)
	{
		throw SdlLoadError(
			"failed to run <" + executorName + "> on resource <" + targetResourceName + "> "
			"(from SDL class: " + clazz.genPrettyName() + ") "
			"-> " + e.whatStr());
	}
}

void SdlParser::parseDirectiveCommand(
	const std::string& command,
	SceneDescription& out_scene)
{
	static const Tokenizer directiveCommandTokenizer(
		{' ', '\t', '\n', '\r'}, 
		{});

	PH_SCOPED_TIMER(ParseDirectiveCommand);

	std::vector<std::string> tokens;
	directiveCommandTokenizer.tokenize(command, tokens);

	// Sanity check: should include additional information other than command type
	if(tokens.size() < 2)
	{
		throw SdlLoadError(
			"empty directive command");
	}

	PH_ASSERT(getCommandType(tokens[0]) == ESdlCommandType::Directive);
	PH_ASSERT_GE(tokens.size(), 2);

	if(tokens[1] == "version")
	{
		if(tokens.size() < 3)
		{
			throw SdlLoadError(
				"no version supplied when specifying PSDL version");
		}

		const std::string_view versionStr = tokens[2];

		if(!m_commandVersion.isInitial())
		{
			PH_LOG_WARNING(SdlParser, "overwriting existing PSDL version: old={}, new={}", 
				m_commandVersion.toString(), versionStr);
		}

		m_commandVersion = SemanticVersion(versionStr);
	}
	else
	{
		throw SdlLoadError(
			"unknown SDL directive: " + tokens[1] + ", ignoring");
	}
}

std::string SdlParser::genNameForAnonymity()
{
	return "@__anonymous-item-" + std::to_string(m_generatedNameCounter++);
}

std::string SdlParser::getName(const std::string_view resourceNameToken)
{
	PH_SCOPED_TIMER(GetName);

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
	PH_SCOPED_TIMER(GetCommandType);

	// Skips any leading whitespace
	const auto headTrimmedSegment = string_utils::trim_head(commandSegment);

	// We cannot know the type yet with too few characters
	if(headTrimmedSegment.size() < 2)
	{
		return ESdlCommandType::Unknown;
	}

	// Test to see if any command symbol is matched
	PH_ASSERT_GE(headTrimmedSegment.size(), 2);
	const auto firstChar = headTrimmedSegment[0];
	const auto secondChar = headTrimmedSegment[1];
	switch(firstChar)
	{
	case '/': 
		// "//": comment
		return secondChar == '/' ? ESdlCommandType::Comment : ESdlCommandType::Unknown;

	case '+':
		// "+>": load
		return secondChar == '>' ? ESdlCommandType::Load : ESdlCommandType::Unknown;

	case '-': 
		// "->": removal
		return secondChar == '>' ? ESdlCommandType::Removal : ESdlCommandType::Unknown;

	case '=': 
		// "=>": update
		return secondChar == '>' ? ESdlCommandType::Update : ESdlCommandType::Unknown;

	case '>':
		// ">>": execution
		return secondChar == '>' ? ESdlCommandType::Execution : ESdlCommandType::Unknown;

	case '#': 
		// "##": directive
		return secondChar == '#' ? ESdlCommandType::Directive : ESdlCommandType::Unknown;

	case 'p':
		// "phantom": invisible object
		return string_utils::trim_tail(headTrimmedSegment) == "phantom"
			? ESdlCommandType::Phantom
			: ESdlCommandType::Unknown;
	}

	return ESdlCommandType::Unknown;
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

	PH_SCOPED_TIMER(GetClauses);

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

	out_clause->type = tokens[0];
	out_clause->payload.value = tokens[2];
	
	// Parse name and an optional tag
	// tokens[1] contains name and tag, syntax: <name>:<optional-tag>

	const std::string_view nameAndTag = tokens[1];
	const auto colonPos = nameAndTag.find(':');
	if(colonPos == std::string_view::npos)
	{
		out_clause->name = tokens[1];
		out_clause->payload.tag = "";
	}
	else
	{
		out_clause->name = std::string(nameAndTag.substr(0, colonPos + 1));
		out_clause->payload.tag = nameAndTag.substr(colonPos);
	}
}

const SdlClass* SdlParser::getSdlClass(const std::string& mangledClassName) const
{
	const auto& iter = m_mangledNameToClass.find(mangledClassName);
	return iter != m_mangledNameToClass.end() ? iter->second : nullptr;
}

const SdlClass& SdlParser::getSdlClass(const std::string_view categoryName, const std::string_view typeName) const
{
	PH_SCOPED_TIMER(GetSDLClass);

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
