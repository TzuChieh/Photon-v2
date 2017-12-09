#pragma once

#include "FileIO/Tokenizer.h"
#include "FileIO/ValueClause.h"
#include "FileIO/FileSystem/Path.h"

#include <vector>
#include <string>
#include <fstream>
#include <unordered_map>

namespace ph
{

class Description;
class CommandEntry;
class SdlTypeInfo;

enum class ECommandType
{
	UNKNOWN, 
	CORE, 
	WORLD, 
	COMMENT
};

class DescriptionParser final
{
public:
	static std::string CORE_DATA_NAME();
	static bool addCommandEntry(const CommandEntry& entry);
	static CommandEntry getCommandEntry(const SdlTypeInfo& typeInfo);

public:
	DescriptionParser();

	void enter(const std::string& commandFragment, Description& out_data);

private:
	Path        m_workingDirectory;
	std::string m_commandCache;
	Tokenizer   m_coreCommandTokenizer;
	Tokenizer   m_worldCommandTokenizer;
	Tokenizer   m_nameTokenizer;
	std::size_t m_generatedNameCounter;

	void parseCommand(const std::string& command, Description& out_data);
	void parseCoreCommand(const std::string& command, Description& out_data);
	void parseWorldCommand(const std::string& command, Description& out_data);
	std::string genName();
	std::string getName(const std::string& nameToken) const;

private:
	bool isResourceName(const std::string& token) const;
	bool isLoadCommand(const std::vector<std::string>& commandTokens) const;
	bool isExecuteCommand(const std::vector<std::string>& commandTokens) const;

	static std::unordered_map<std::string, CommandEntry>& NAMED_INTERFACE_MAP();
	static std::string getFullTypeName(const SdlTypeInfo& typeInfo);
	static ECommandType getCommandType(const std::string& command);
	static std::vector<ValueClause> getValueClauses(const std::vector<std::string>& clauseStrings);
};

}// end namespace ph