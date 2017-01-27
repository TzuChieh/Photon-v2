#pragma once

#include "Actor/AModel.h"
#include "FileIO/Tokenizer.h"
#include "FileIO/NamedResourceCache.h"
#include "FileIO/ValueClause.h"

#include <vector>
#include <string>
#include <fstream>

namespace ph
{

class Description;

enum class ECommandType
{
	UNKNOWN, 
	CORE, 
	WORLD, 
	COMMENT
};

class DescriptionFileParser final
{
public:
	DescriptionFileParser();

	bool load(const std::string& fullFilename, Description* const out_data);

private:
	NamedResourceCache m_cache;
	Tokenizer m_coreCommandTokenizer;
	Tokenizer m_worldCommandTokenizer;
	std::size_t m_generatedNameCounter;

	void parseCoreCommand(const std::string& command, Description& out_data);
	void parseWorldCommand(const std::string& command);
	void populateWorldWithActors(Description& out_data);
	std::string genName();
	std::string getName(const std::string& nameToken);

	static void getCommandString(std::ifstream& dataFile, std::string* const out_command, ECommandType* const out_type);
	static ECommandType getCommandType(const std::string& command);
	static std::vector<ValueClause> getValueClauses(const std::vector<std::string>& clauseStrings);
};

}// end namespace ph