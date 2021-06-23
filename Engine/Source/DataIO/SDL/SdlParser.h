#pragma once

#include "DataIO/SDL/ValueClauses.h"
#include "DataIO/FileSystem/Path.h"

#include <vector>
#include <string>
#include <string_view>
#include <unordered_map>

namespace ph
{

class SceneDescription;
class SdlClass;

enum class ESdlCommandType
{
	UNKNOWN,
	DIRECTIVE,
	LOAD,
	REMOVAL,
	EXECUTION,
	COMMENT
};

class SdlParser final
{
public:
	SdlParser();

	// TODO: rename existing methods to enterAndWait() and flushAndWait() and add flush() and enter() once multithreading is added

	/*! @brief Enters a segment of command and parse it once a command is complete.

	The command segment must be valid. The method will cache the command
	segment in subsequent calls until a command is complete, at which point
	the command will be parsed and the result will act on the provided scene.
	
	A valid command segment is a portion of a complete command that do not 
	break any keyword or symbol of SDL. For example, you can break any
	opening/closing braces, but you cannot break a type name.

	@param commandSegment A valid segment of command.
	@param[out] out_scene The target for parsed results.
	*/
	void enter(std::string_view commandSegment, SceneDescription& out_scene);

	/*! @brief Force the parse of commands that were cached in the parser.

	@param[out] out_scene The target for parsed results.
	*/
	void flush(SceneDescription& out_scene);

	void setWorkingDirectory(const Path& path);

private:
	std::unordered_map<std::string, const SdlClass*> m_mangledNameToClass;

	Path        m_workingDirectory;
	std::string m_commandCache;
	std::size_t m_generatedNameCounter;

	void parseCommand(const std::string& command, SceneDescription& out_scene);
	void parseSingleCommand(ESdlCommandType type, const std::string& command, SceneDescription& out_scene);

	void parseLoadCommand( 
		const std::string& command,
		SceneDescription&  out_scene);

	void parseExecutionCommand(
		const std::string& command,
		SceneDescription&  out_scene);

	std::string getName(std::string_view resourceNameToken);
	std::string genNameForAnonymity();

private:
	const SdlClass* getSdlClass(const std::string& mangledClassName) const;
	const SdlClass& getSdlClass(std::string_view categoryName, std::string_view typeName) const;

	static std::string getMangledName(std::string_view categoryName, std::string_view typeName);
	static void getMangledName(std::string_view categoryName, std::string_view typeName, std::string* out_mangledName);
	static void getClauses(const std::vector<std::string>& clauseStrings, ValueClauses* out_clauses);
	static void getClause(std::string_view clauseString, ValueClauses::Clause* out_clause);
	static ESdlCommandType getCommandType(std::string_view commandSegment);
};

}// end namespace ph
