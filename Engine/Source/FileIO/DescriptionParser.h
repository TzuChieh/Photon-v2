#pragma once

#include "Actor/AModel.h"
#include "FileIO/Tokenizer.h"
#include "FileIO/ValueClause.h"

#include <vector>
#include <string>
#include <fstream>
#include <unordered_map>

namespace ph
{

class Description;
class CommandEntry;

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
	static bool addCommandEntry(const CommandEntry& entry);

private:
	static std::unordered_map<std::string, CommandEntry>& NAMED_INTERFACE_MAP();

public:
	DescriptionParser();

	void enter(const std::string& commandFragment, Description& out_data);
	//void populateWorldWithActors(Description& out_data);

private:
	std::string        m_commandCache;
	Tokenizer          m_coreCommandTokenizer;
	Tokenizer          m_worldCommandTokenizer;
	std::size_t        m_generatedNameCounter;

	void parseCommand(const std::string& command, Description& out_data);
	void parseCoreCommand(const std::string& command, Description& out_data);
	void parseWorldCommand(const std::string& command, Description& out_data);
	std::string genName();
	std::string getName(const std::string& nameToken);

	static void getCommandString(std::ifstream& dataFile, std::string* const out_command, ECommandType* const out_type);
	static ECommandType getCommandType(const std::string& command);
	static std::vector<ValueClause> getValueClauses(const std::vector<std::string>& clauseStrings);
};

}// end namespace ph