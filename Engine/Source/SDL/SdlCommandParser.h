#pragma once

#include "SDL/SdlInputClauses.h"
#include "SDL/ESdlTypeCategory.h"
#include "SDL/SdlInlinePacketInterface.h"
#include "DataIO/FileSystem/Path.h"
#include "Utility/SemanticVersion.h"
#include "Utility/TSpan.h"

#include <string>
#include <string_view>
#include <unordered_map>

namespace ph
{

class SdlDataPacketInterface;

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

class SdlCommandParser
{
public:
	explicit SdlCommandParser(TSpanView<const SdlClass*> targetClasses);
	SdlCommandParser(TSpanView<const SdlClass*> targetClasses, const Path& sceneWorkingDirectory);
	virtual ~SdlCommandParser();

	// TODO: rename existing methods to enterAndWait() and flushAndWait() and add flush() and enter() once multithreading is added
	// TODO: removeResource() callback for load error, etc.

	virtual SdlDataPacketInterface& getPacketInterface();

	/*! @brief Enters a string and parse it as one or more commands.

	The command segment must have valid syntax. The method will potentially cache the command
	segment in subsequent calls until a command is complete, at which point the command will 
	be parsed and the result will act on the target (e.g., a scene, depending on the implementation).
	
	A valid command segment is a portion of a complete command or a chunk of multiple commands 
	that do not break any keyword or symbol of PSDL. For example, you can break any
	opening/closing braces, but you cannot break a type name.

	@param commandSegment A valid segment of command.
	*/
	void parse(std::string_view rawCommandSegment);

	/*! @brief Force the parse of commands that were cached in the parser.
	*/
	void flush();

	const Path& getSceneWorkingDirectory() const;
	void setSceneWorkingDirectory(const Path& directory);

	const SemanticVersion& getCommandVersion() const;
	std::size_t numParsedCommands() const;
	std::size_t numParseErrors() const;

protected:
	virtual bool beginCommand(ESdlCommandType commandType, const SdlClass* targetClass) = 0;

	virtual ISdlResource* createResource(
		std::string_view resourceName, 
		const SdlClass* resourceClass, 
		ESdlCommandType commandType) = 0;
	
	virtual void initResource(
		ISdlResource* resource, 
		const SdlClass* resourceClass,
		std::string_view resourceName,
		SdlInputClauses& clauses,
		ESdlCommandType commandType) = 0;
	
	virtual ISdlResource* getResource(std::string_view resourceName, ESdlTypeCategory category) = 0;

	virtual void runExecutor(
		std::string_view executorName,
		const SdlClass* targetClass,
		ISdlResource* targetResource,
		SdlInputClauses& clauses,
		ESdlCommandType commandType) = 0;

	virtual void commandVersionSet(const SemanticVersion& version) = 0;
	virtual void endCommand() = 0;

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

	Path m_sceneWorkingDirectory;
	SdlInlinePacketInterface m_inlinePacketInterface;
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
	void enterProcessed(std::string_view processedCommandSegment);

	void parseCommand(const std::string& command);
	void parseSingleCommand(const CommandHeader& command);

	void parseLoadCommand(const CommandHeader& command);
	void parseExecutionCommand(const CommandHeader& command);
	void parseDirectiveCommand(const CommandHeader& command);

	/*! @brief Retrieve clauses from the packet command.
	*/
	void getClauses(
		std::string_view packetCommand, 
		const SdlClass* targetClass,
		std::string_view targetName,
		ISdlResource* targetInstance,
		SdlInputClauses* out_clauses);

	std::string getName(std::string_view referenceToken);
	std::string genNameForAnonymity();

	const SdlClass* getSdlClass(const std::string& mangledClassName) const;
	const SdlClass& getSdlClass(std::string_view categoryName, std::string_view typeName) const;

private:
	static std::string getMangledName(std::string_view categoryName, std::string_view typeName);
	static void getMangledName(std::string_view categoryName, std::string_view typeName, std::string* out_mangledName);
	static CommandHeader parseCommandHeader(std::string_view command);
};

// In-header Implementations:

inline const Path& SdlCommandParser::getSceneWorkingDirectory() const
{
	return m_sceneWorkingDirectory;
}

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