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
	REGULAR,
	COMMENT
};

class SdlParser final
{
public:
	static bool addCommandEntry(const CommandEntry& entry);
	static CommandEntry getCommandEntry(const SdlTypeInfo& typeInfo);

public:
	SdlParser();

	void enter(const std::string& commandFragment, SdlResourcePack& out_pack);
	void setWorkingDirectory(const Path& path);

private:
	Path        m_workingDirectory;
	std::string m_commandCache;
	Tokenizer   m_regularCommandTokenizer;
	Tokenizer   m_nameTokenizer;
	std::size_t m_generatedNameCounter;

	void parseCommand(const std::string& command, SdlResourcePack& out_pack);

	bool parseRegularCommand(
		ECommandType       type,
		const std::string& command, 
		SdlResourcePack&   out_pack);

	bool parseLoadCommand(
		ECommandType                    type,     
		const std::vector<std::string>& tokens, 
		SdlResourcePack&                out_pack);

	bool parseExecuteCommand(
		ECommandType                    type,
		const std::vector<std::string>& tokens, 
		SdlResourcePack&                out_pack);

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
