#pragma once

#include "SDL/SdlInputClauses.h"
#include "DataIO/FileSystem/Path.h"
#include "Utility/SemanticVersion.h"

#include <vector>
#include <string>
#include <string_view>
#include <unordered_map>

namespace ph
{

class SceneDescription;
class SdlCommandParser;

enum class ESdlCommandType
{
	Unknown = 0,
	Directive,
	Load,
	Removal,
	Update,
	Execution,
	Phantom
};

class SdlCommandParser final
{
public:
	SdlCommandParser();

	// TODO: rename existing methods to enterAndWait() and flushAndWait() and add flush() and enter() once multithreading is added

	/*! @brief Enters a string and parse it as one or more commands.

	The command segment must have valid syntax. The method will potentially cache the command
	segment in subsequent calls until a command is complete, at which point the command will 
	be parsed and the result will act on the provided scene.
	
	A valid command segment is a portion of a complete command or a chunk of multiple commands 
	that do not break any keyword or symbol of PSDL. For example, you can break any
	opening/closing braces, but you cannot break a type name.

	@param commandSegment A valid segment of command.
	@param[out] out_scene The target for parsed results.
	*/
	void enter(std::string_view rawCommandSegment, SceneDescription& out_scene);

	/*! @brief Force the parse of commands that were cached in the parser.

	@param[out] out_scene The target for parsed results.
	*/
	void flush(SceneDescription& out_scene);

	void setWorkingDirectory(const Path& path);

	const SemanticVersion& getCommandVersion() const;
	std::size_t numParsedCommands() const;
	std::size_t numParseErrors() const;

private:
	// OPT: use view
	struct CommandHeader final
	{
		ESdlCommandType commandType = ESdlCommandType::Unknown;
		std::string targetCategory;
		std::string targetType;
		std::string executorName;
		std::string reference;
		std::string dataString;

		bool isRecognized() const;
	};

	SemanticVersion m_commandVersion;

	std::unordered_map<std::string, const SdlClass*> m_mangledNameToClass;

	Path m_workingDirectory;
	bool m_isInSingleLineComment;
	std::string m_processedCommandCache;
	std::size_t m_generatedNameCounter;
	std::size_t m_numParsedCommands;
	std::size_t m_numParseErrors;

private:
	/*!
	@param processedCommandSegment A command segment with all pre-processing being done (e.g., 
	comment string removal).
	*/
	void enterProcessed(std::string_view processedCommandSegment, SceneDescription& out_scene);

	void parseCommand(const std::string& command, SceneDescription& out_scene);
	void parseSingleCommand(const CommandHeader& command, SceneDescription& out_scene);

	void parseLoadCommand(
		const CommandHeader& command,
		SceneDescription& out_scene);

	void parseExecutionCommand(
		const CommandHeader& command,
		SceneDescription& out_scene);

	void parseDirectiveCommand(
		const CommandHeader& command,
		SceneDescription& out_scene);

	std::string getName(std::string_view referenceToken);
	std::string genNameForAnonymity();

	const SdlClass* getSdlClass(const std::string& mangledClassName) const;
	const SdlClass& getSdlClass(std::string_view categoryName, std::string_view typeName) const;

private:
	static std::string getMangledName(std::string_view categoryName, std::string_view typeName);
	static void getMangledName(std::string_view categoryName, std::string_view typeName, std::string* out_mangledName);
	static void getClauses(std::string_view clauseString, SdlInputClauses* out_clauses);
	static void getClauses(const std::vector<std::string>& clauseStrings, SdlInputClauses* out_clauses);
	static void getSingleClause(std::string_view clauseString, SdlInputClause* out_clause);
	static CommandHeader parseCommandHeader(std::string_view command);
};

// In-header Implementations:

inline const SemanticVersion& SdlCommandParser::getCommandVersion() const
{
	return m_commandVersion;
}

inline std::size_t SdlCommandParser::numParsedCommands() const
{
	return m_numParsedCommands;
}

inline std::size_t SdlCommandParser::numParseErrors() const
{
	return m_numParseErrors;
}

inline bool SdlCommandParser::CommandHeader::isRecognized() const
{
	return commandType != ESdlCommandType::Unknown;
}

}// end namespace ph
