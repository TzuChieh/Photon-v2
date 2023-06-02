#include "SDL/SdlCommandParser.h"
#include "SDL/Tokenizer.h"
#include "Common/logging.h"
#include "SDL/sdl_helpers.h"
#include "SDL/ESdlTypeCategory.h"
#include "Utility/string_utils.h"
#include "SDL/Introspect/SdlClass.h"
#include "SDL/sdl_exceptions.h"
#include "SDL/Introspect/SdlInputContext.h"
#include "Common/stats.h"
#include "Common/config.h"

#include <cstddef>
#include <utility>
#include <vector>

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(SdlCommandParser, SDL);
PH_DEFINE_INTERNAL_TIMER_STAT(ParseCommandTotal, SdlCommandParser);
PH_DEFINE_INTERNAL_TIMER_STAT(ParseLoadCommand, SdlCommandParser);
PH_DEFINE_INTERNAL_TIMER_STAT(ParseExecutionCommand, SdlCommandParser);
PH_DEFINE_INTERNAL_TIMER_STAT(ParseDirectiveCommand, SdlCommandParser);
PH_DEFINE_INTERNAL_TIMER_STAT(GetCommandHeader, SdlCommandParser);
PH_DEFINE_INTERNAL_TIMER_STAT(GetName, SdlCommandParser);
PH_DEFINE_INTERNAL_TIMER_STAT(GetClauses, SdlCommandParser);
PH_DEFINE_INTERNAL_TIMER_STAT(GetSDLClass, SdlCommandParser);

SdlCommandParser::SdlCommandParser(TSpanView<const SdlClass*> targetClasses)
	: SdlCommandParser(targetClasses, Path("./"))
{}

SdlCommandParser::SdlCommandParser(
	TSpanView<const SdlClass*> targetClasses, 
	const Path& sceneWorkingDirectory)

	: m_commandVersion(PH_PSDL_VERSION)
	, m_mangledNameToClass()
	, m_sceneWorkingDirectory(sceneWorkingDirectory)
	, m_inlinePacketInterface(sceneWorkingDirectory)
	, m_isInSingleLineComment(false)
	, m_processedCommandCache()
	, m_generatedNameCounter(0)
	, m_numParsedCommands(0)
	, m_numParseErrors(0)
{
	for(const SdlClass* clazz : targetClasses)
	{
		if(!clazz)
		{
			PH_LOG_WARNING(SdlCommandParser, 
				"null SDL target class detected during parser construction");
			continue;
		}

		const auto mangledClassName = getMangledName(clazz->genCategoryName(), clazz->getTypeName());
		const auto& iter = m_mangledNameToClass.find(mangledClassName);
		if(iter != m_mangledNameToClass.end())
		{
			PH_LOG_WARNING(SdlCommandParser,
				"SDL class <{}> already registered, overwriting; please check for name "
				"collision: mangled name is <{}>", 
				clazz->genPrettyName(), mangledClassName);
		}

		m_mangledNameToClass[mangledClassName] = clazz;
	}
}

SdlCommandParser::~SdlCommandParser() = default;

SdlDataPacketInterface& SdlCommandParser::getPacketInterface()
{
	return m_inlinePacketInterface;
}

void SdlCommandParser::parse(std::string_view rawCommandSegment)
{
	// TODO: we may need to preprocess string enclosures ("") here too (think "//" inside a string)

	// Process the segment line by line, while stripping out comment sections. 
	// Note: do not trim here as some whitespaces are part of the syntax (e.g., newline as the
	// delimiter for single-line comment)

	std::string_view remainingSegment = rawCommandSegment;
	while(!remainingSegment.empty())
	{
		if(m_isInSingleLineComment)
		{
			const auto newlinePos = remainingSegment.find('\n');
			if(newlinePos == std::string_view::npos)
			{
				// Skip all since we require '\n' to end the single-line comment
				remainingSegment = "";
			}
			else
			{
				// Still need to submit the newline char as it is part of the syntax (separator)
				enterProcessed("\n");

				remainingSegment = remainingSegment.substr(newlinePos + 1);
				m_isInSingleLineComment = false;
			}
		}
		else
		{
			const auto keyCharPos = remainingSegment.find_first_of("\n" "/");
			if(keyCharPos == std::string_view::npos)
			{
				// Submit all since no special character is met
				PH_ASSERT(!m_isInSingleLineComment);
				enterProcessed(remainingSegment);
				remainingSegment = "";
			}
			else
			{
				const auto keyChar = remainingSegment[keyCharPos];
				switch(keyChar)
				{
				case '\n':
				{
					// Submit with the newline as it is part of the syntax (separator)
					PH_ASSERT(!m_isInSingleLineComment);
					enterProcessed(remainingSegment.substr(0, keyCharPos + 1));

					remainingSegment = remainingSegment.substr(keyCharPos + 1);
					break;
				}
				
				case '/':
				{
					// The part before slash should be submitted anyway
					std::size_t numCharsToSubmit = keyCharPos;

					const bool isNextCharSlash = 
						keyCharPos + 1 < remainingSegment.size() &&
						remainingSegment[keyCharPos + 1] == '/';

					// Requires two slashes for a single-line comment
					if(isNextCharSlash)
					{
						PH_ASSERT(!m_isInSingleLineComment);
						m_isInSingleLineComment = true;
					}
					// Just a standalone slash, then it should be part of the command
					else
					{
						// One for the standalone slash and the other is the peeked char
						numCharsToSubmit += 2;
					}

					enterProcessed(remainingSegment.substr(0, numCharsToSubmit));

					// Note that either way we peeked and processed the next char, hence +2
					remainingSegment = remainingSegment.substr(keyCharPos + 2);
					break;
				}

				default:
					PH_ASSERT_UNREACHABLE_SECTION();
					break;
				}
			}
		}
	}// end while segment is not empty
}

void SdlCommandParser::enterProcessed(std::string_view processedCommandSegment)
{
	// Note: a `processedCommandSegment` may contain zero to multiple commands, and the command 
	// may be incomplete

	std::string_view remainingSegment = string_utils::trim(processedCommandSegment);
	while(!remainingSegment.empty())
	{
		// Input string is already pre-processed, here we just need to find the command delimiter
		const auto semicolonPos = remainingSegment.find(';');
		if(semicolonPos == std::string_view::npos)
		{
			// Cache all since the command is not finished yet
			m_processedCommandCache += remainingSegment;
			remainingSegment = "";
		}
		else
		{
			// Excluding the semicolon
			m_processedCommandCache += remainingSegment.substr(0, semicolonPos);
			remainingSegment = remainingSegment.substr(semicolonPos + 1);

			// Flush when a full command is entered
			flush();
		}
	}// end while segment is not empty
}

void SdlCommandParser::flush()
{
	// OPT: use view
	parseCommand(m_processedCommandCache);

	m_processedCommandCache.clear();
	m_processedCommandCache.shrink_to_fit();// TODO: reconsider, maybe only reset if too large
}

void SdlCommandParser::parseCommand(const std::string& command)
{
	PH_SCOPED_TIMER(ParseCommandTotal);

	if(command.empty())
	{
		return;
	}

	try
	{
		const CommandHeader header = parseCommandHeader(command);
		if(!header.isRecognized())
		{
			throw SdlLoadError(
				"unrecognizable command type");
		}

		parseSingleCommand(header);
	}
	catch(const SdlLoadError& e)
	{
		// Make a shorter version of the command in case the data string is large
		std::string shortenedCommand = command.size() > 64
			? command.substr(0, 64) + " (reduced due to length...)"
			: command;
		shortenedCommand = string_utils::trim(shortenedCommand);

		PH_LOG_WARNING(SdlCommandParser,
			"command failed to run -> {} (parsing <{}>)", 
			e.whatStr(), shortenedCommand);

		++m_numParseErrors;
	}
}

void SdlCommandParser::parseSingleCommand(const CommandHeader& command)
{
	switch(command.commandType)
	{
	case ESdlCommandType::Load:
	case ESdlCommandType::Phantom:
		parseLoadCommand(command);
		break;

	case ESdlCommandType::Execution:
		parseExecutionCommand(command);
		break;

	case ESdlCommandType::Directive:
		parseDirectiveCommand(command);
		break;

	default:
		throw SdlLoadError("unsupported command type");
		break;
	}

	++m_numParsedCommands;
}

void SdlCommandParser::parseLoadCommand(const CommandHeader& command)
{
	PH_SCOPED_TIMER(ParseLoadCommand);

	if(command.reference.empty())
	{
		throw SdlLoadError(
			"syntax error: resource name is required in a load command");
	}

	PH_ASSERT(
		command.commandType == ESdlCommandType::Load ||
		command.commandType == ESdlCommandType::Phantom);

	// Get category and type then acquire the matching SDL class
	const SdlClass& clazz = getSdlClass(command.targetCategory, command.targetType);

	const std::string& resourceName = getName(command.reference);

	// Now we have name-related information, which is useful for debugging. 
	// Catch load errors here to provide name information and re-throw.
	try
	{
		if(!beginCommand(command.commandType, &clazz))
		{
			return;
		}

		ISdlResource* resource = createResource(resourceName, &clazz, command.commandType);
		if(!resource)
		{
			throw SdlLoadError("empty resource generated");
		}

		// Initialize SDL resource from input value clauses

		// TODO: reuse clause buffer
		SdlInputClauses clauses;
		getClauses(command.dataString, &clazz, resourceName, resource, &clauses);

		initResource(
			resource, 
			&clazz,
			resourceName,
			clauses,
			command.commandType);

		endCommand();
	}
	catch(const SdlLoadError& e)
	{
		throw SdlLoadError(
			"failed to load resource <" + resourceName + "> "
			"(from SDL class: " + clazz.genPrettyName() + ") "
			"-> " + e.whatStr());
	}
}

void SdlCommandParser::parseExecutionCommand(const CommandHeader& command)
{
	PH_SCOPED_TIMER(ParseExecutionCommand);

	PH_ASSERT(command.commandType == ESdlCommandType::Execution);

	// Get category and type then acquire the matching SDL class
	const SdlClass& clazz = getSdlClass(command.targetCategory, command.targetType);

	const std::string& targetResourceName = getName(command.reference);
	const std::string& executorName = command.executorName;

	// Now we have name-related information, which is useful for debugging. 
	// Catch load errors here to provide name information and re-throw.
	try
	{
		if(!beginCommand(command.commandType, &clazz))
		{
			return;
		}

		// Get target SDL resource and clauses

		ISdlResource* resource = getResource(targetResourceName, clazz.getCategory());

		// TODO: reuse clause buffer
		SdlInputClauses clauses;
		getClauses(command.dataString, &clazz, targetResourceName, resource, &clauses);

		// Finally, call the executor

		runExecutor(
			executorName,
			&clazz,
			resource,
			clauses,
			command.commandType);

		endCommand();
	}
	catch(const SdlLoadError& e)
	{
		throw SdlLoadError(
			"failed to run <" + executorName + "> on resource <" + targetResourceName + "> "
			"(from SDL class: " + clazz.genPrettyName() + ") "
			"-> " + e.whatStr());
	}
}

void SdlCommandParser::parseDirectiveCommand(const CommandHeader& command)
{
	static const Tokenizer directiveTokenizer(
		{' ', '\t', '\n', '\r'}, 
		{});

	PH_SCOPED_TIMER(ParseDirectiveCommand);

	std::string_view directiveString = command.dataString;
	directiveString = string_utils::trim(directiveString);

	// OPT: use view
	std::vector<std::string> tokens;
	directiveTokenizer.tokenize(std::string(directiveString), tokens);

	// Sanity check: should include additional information other than command type
	if(tokens.empty())
	{
		throw SdlLoadError(
			"empty directive command");
	}

	PH_ASSERT(command.commandType == ESdlCommandType::Directive);
	PH_ASSERT(!tokens.empty());

	if(!beginCommand(command.commandType, nullptr))
	{
		return;
	}

	if(tokens[0] == "version")
	{
		if(tokens.size() < 2)
		{
			throw SdlLoadError(
				"no version supplied when specifying PSDL version");
		}

		const std::string_view versionStr = tokens[1];
		const SemanticVersion loadedVersion(versionStr);

		if(loadedVersion != m_commandVersion)
		{
			PH_LOG_WARNING(SdlCommandParser,
				"switching PSDL version: old={}, new={} (engine native PSDL={})", 
				m_commandVersion.toString(), versionStr, PH_PSDL_VERSION);
		}

		m_commandVersion = loadedVersion;
		commandVersionSet(loadedVersion);
	}
	else
	{
		throw SdlLoadError(
			"unknown SDL directive: " + tokens[0] + ", ignoring");
	}

	endCommand();
}

void SdlCommandParser::getClauses(
	std::string_view packetCommand, 
	const SdlClass* const targetClass,
	std::string_view targetName,
	ISdlResource* const targetInstance,
	SdlInputClauses* const out_clauses)
{
	PH_SCOPED_TIMER(GetClauses);

	PH_ASSERT(out_clauses);

	out_clauses->clear();
	getPacketInterface().parse(
		packetCommand, 
		targetClass,
		targetName,
		targetInstance,
		*out_clauses);
}

std::string SdlCommandParser::genNameForAnonymity()
{
	return "@__anonymous-item-" + std::to_string(m_generatedNameCounter++);
}

std::string SdlCommandParser::getName(const std::string_view referenceToken)
{
	PH_SCOPED_TIMER(GetName);

	// Remove any leading and trailing blank characters
	const auto trimmedToken = string_utils::trim(referenceToken);

	// Should at least contain a '@' character
	if(trimmedToken.empty())
	{
		throw_formatted<SdlLoadError>(
			"syntax error: reference is empty, <{}> was given",
			referenceToken);
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
			PH_ASSERT_EQ(trimmedToken.size(), 1);
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
			throw_formatted<SdlLoadError>(
				"syntax error: resource name missing ending double quote and/or the @ prefix, "
				"<{}> was given",
				referenceToken);
		}

	default:
		throw_formatted<SdlLoadError>(
			"syntax error: resource name should start with @, optionally "
			"enclosed by double quotes, <{}> was given",
			referenceToken);
	}
}

void SdlCommandParser::setSceneWorkingDirectory(const Path& directory)
{
	m_sceneWorkingDirectory = directory;
}

auto SdlCommandParser::parseCommandHeader(const std::string_view command)
-> CommandHeader
{
	static const Tokenizer commandTokenizer(
		{' ', '\t', '\n', '\r'},
		{{'\"', '\"'}, {'(', ')'}});

	PH_SCOPED_TIMER(GetCommandHeader);

	// Skips any leading whitespace
	const auto headTrimmedCommand = string_utils::trim_head(command);

	// Require at least 2 characters, e.g., `#?` or `//`
	if(headTrimmedCommand.size() < 2)
	{
		PH_LOG_WARNING(SdlCommandParser, "invalid command detected: {}", command);
		return CommandHeader();
	}

	CommandHeader header;

	// Test to see if any simple symbol sequence is matched
	PH_ASSERT_GE(headTrimmedCommand.size(), 2);
	const auto firstChar = headTrimmedCommand[0];
	const auto secondChar = headTrimmedCommand[1];
	switch(firstChar)
	{
	case '#':
		// "#": directive
		header.commandType = ESdlCommandType::Directive;
		header.dataString = headTrimmedCommand.substr(1);
		break;
	}

	if(header.commandType != ESdlCommandType::Unknown)
	{
		return header;
	}

	// Parsing resource command
	
	const auto equalSignPos = headTrimmedCommand.find('=');
	if(equalSignPos == std::string_view::npos)
	{
		throw SdlLoadError(
			"syntax error: resource command requires an assignment operator, none was found");
	}

	const auto headerString = string_utils::trim_tail(headTrimmedCommand.substr(0, equalSignPos));
	header.dataString = headTrimmedCommand.substr(equalSignPos + 1);

	const auto dotSignPos = headerString.find('.');

	// No dot sign
	if(dotSignPos == std::string_view::npos)
	{
		// OPT: use view
		std::vector<std::string> tokens;
		commandTokenizer.tokenize(std::string(headerString), tokens);

		switch(tokens.size())
		{
		case 1:
			// Executor call without SDL type and reference, e.g., `Func()`
			header.commandType = ESdlCommandType::Execution;
			header.executorName = tokens[0];
			break;

		case 2:
			// Executor call without SDL type but with reference, e.g., `Func(@Ref)`
			header.commandType = ESdlCommandType::Execution;
			header.executorName = tokens[0];
			header.reference = tokens[1];
			break;

		case 3:
			// Creator with SDL type and reference, e.g., `Category(Type) @Ref`
			header.commandType = ESdlCommandType::Load;
			header.targetCategory = tokens[0];
			header.targetType = tokens[1];
			header.reference = tokens[2];
			break;

		case 4:
			// Phantom with SDL type and reference, e.g., `phantom Category(Type) @Ref`
			if(tokens[0] == "phantom")
			{
				header.commandType = ESdlCommandType::Phantom;
				header.targetCategory = tokens[1];
				header.targetType = tokens[2];
				header.reference = tokens[3];
			}
			else
			{
				throw_formatted<SdlLoadError>(
					"unknown keyword <{}> found in creator command",
					tokens[0]);
			}
			break;

		default:
			return CommandHeader();
		}
	}
	// With dot sign
	else
	{
		const auto dotSignLhsString = headerString.substr(0, dotSignPos);
		const auto dotSignRhsString = headerString.substr(dotSignPos + 1);

		// OPT: use view
		std::vector<std::string> dotSignLhsTokens;
		commandTokenizer.tokenize(std::string(dotSignLhsString), dotSignLhsTokens);

		// OPT: use view
		std::vector<std::string> dotSignRhsTokens;
		commandTokenizer.tokenize(std::string(dotSignRhsString), dotSignRhsTokens);

		// Two possibilities here:
		// (1) Executor call with SDL type but without reference, e.g., `Category(Type).Func()`
		// (2) Executor call with SDL type and reference, e.g., `Category(Type).Func(@Ref)`

		header.commandType = ESdlCommandType::Execution;

		// LHS is always category and type info, e.g., `Category(Type)`
		if(dotSignLhsTokens.size() == 2)
		{
			header.targetCategory = dotSignLhsTokens[0];
			header.targetType = dotSignLhsTokens[1];
		}
		else
		{
			throw_formatted<SdlLoadError>(
				"invalid category and type info <{}> found in executor command (explicit form)",
				dotSignLhsString);
		}

		switch(dotSignRhsTokens.size())
		{
		case 1:
			// Executor call without reference, e.g., `Func()`
			header.executorName = dotSignRhsTokens[0];
			break;

		case 2:
			// Executor call with reference, e.g., `Func(@Ref)`
			header.executorName = dotSignRhsTokens[0];
			header.reference = dotSignRhsTokens[1];
			break;

		default:
			throw_formatted<SdlLoadError>(
				"invalid call target <{}> found in executor command (explicit form)",
				dotSignRhsString);
		}
	}

	return header;
}

std::string SdlCommandParser::getMangledName(const std::string_view categoryName, const std::string_view typeName)
{
	// OPT: no alloc
	std::string mangledName;
	getMangledName(categoryName, typeName, &mangledName);
	return mangledName;
}

void SdlCommandParser::getMangledName(const std::string_view categoryName, const std::string_view typeName, std::string* const out_mangledName)
{
	PH_ASSERT(out_mangledName);

	out_mangledName->clear();
	*out_mangledName += std::string(categoryName) + std::string(typeName);
}

const SdlClass* SdlCommandParser::getSdlClass(const std::string& mangledClassName) const
{
	const auto& iter = m_mangledNameToClass.find(mangledClassName);
	return iter != m_mangledNameToClass.end() ? iter->second : nullptr;
}

const SdlClass& SdlCommandParser::getSdlClass(const std::string_view categoryName, const std::string_view typeName) const
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