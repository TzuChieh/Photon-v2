#pragma once

#include "DataIO/SDL/Tokenizer.h"
#include "DataIO/SDL/ValueClause.h"
#include "DataIO/FileSystem/Path.h"

#include <vector>
#include <string>
#include <fstream>
#include <unordered_map>

namespace ph
{

class SdlResourcePack;
class CommandEntry;
class SdlTypeInfo;

enum class ECommandType
{
	UNKNOWN, 
	CORE, 
	WORLD, 
	COMMENT
};

class SdlParser final
{
public:
	static std::string CORE_DATA_NAME();
	static bool addCommandEntry(const CommandEntry& entry);
	static CommandEntry getCommandEntry(const SdlTypeInfo& typeInfo);

public:
	SdlParser();

	void enter(const std::string& commandFragment, SdlResourcePack& out_data);
	void setWorkingDirectory(const Path& path);

private:
	Path        m_workingDirectory;
	std::string m_commandCache;
	Tokenizer   m_coreCommandTokenizer;
	Tokenizer   m_worldCommandTokenizer;
	Tokenizer   m_nameTokenizer;
	std::size_t m_generatedNameCounter;

	void parseCommand(const std::string& command, SdlResourcePack& out_data);

	bool parseRegularCommand(
		ECommandType       type,
		const std::string& command, 
		SdlResourcePack&   out_data);

	bool parseLoadCommand(
		ECommandType                    type,     
		const std::vector<std::string>& tokens, 
		SdlResourcePack&                out_data);

	bool parseExecuteCommand(
		ECommandType                    type,
		const std::vector<std::string>& tokens, 
		SdlResourcePack&                out_data);

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
