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

class SceneDescription;

enum class ECommandType
{
	UNKNOWN, 
	CORE,
	WORLD
};

class RenderingFileParser final
{
public:
	RenderingFileParser();

	bool load(const std::string& fullFilename, SceneDescription* const out_data);

private:
	NamedResourceCache m_cache;
	Tokenizer m_worldCommandTokenizer;
	std::size_t m_generatedNameCounter;

	void parseCoreCommand(const std::string& command);
	void parseWorldCommand(const std::string& command);
	std::string genName();
	std::string getName(const std::string& nameToken);

	static void getCommandString(std::ifstream& dataFile, std::string* const out_command, ECommandType* const out_type);
	static ECommandType getCommandType(const std::string& command);
	static std::vector<ValueClause> getValueClauses(const std::vector<std::string>& clauseStrings);
};

}// end namespace ph