#include "FileIO/RenderingFileParser.h"
#include "FileIO/SceneDescription.h"
#include "FileIO/resource_loading.h"

#include <iostream>
#include <sstream>

namespace ph
{

RenderingFileParser::RenderingFileParser() :
	m_worldCommandTokenizer({' ', '\t', '\n', '\r'}, {{'\"', '\"'}, {'[', ']'}, {'<', '>'}}), 
	m_generatedNameCounter(0)
{

}

bool RenderingFileParser::load(const std::string& fullFilename, SceneDescription* const out_data)
{
	std::ifstream dataFile;
	dataFile.open(fullFilename, std::ios::in);
	if(!dataFile.is_open())
	{
		std::cerr << "warning: at RenderingFileLoader::load(), failed to open file <" + fullFilename + ">" << std::endl;
		return false;
	}
	else
	{
		ECommandType commandType;
		std::string commandString;
		while(true)
		{
			getCommandString(dataFile, &commandString, &commandType);

			if(commandType == ECommandType::WORLD)
			{
				parseWorldCommand(commandString);
			}
			else if(commandType == ECommandType::CORE)
			{
				parseCoreCommand(commandString);
			}
			else
			{
				break;
			}
		}

		dataFile.close();
		return true;
	}
}

void RenderingFileParser::parseCoreCommand(const std::string& command)
{
	std::cout << "core command: " << command << std::endl;
}

void RenderingFileParser::parseWorldCommand(const std::string& command)
{
	std::vector<std::string> tokens;
	m_worldCommandTokenizer.tokenize(command, tokens);

	if(tokens.size() < 4)
	{
		std::cerr << "warning: at RenderingFileLoader::parseWorldCommand(), bad formatted command <" + command + ">" << std::endl;
		return;
	}

	const std::string commandName = tokens[1];
	if(commandName == "geometry")
	{
		const std::vector<std::string> clauseStrings(tokens.begin() + 3, tokens.end());
		m_cache.addGeometry(getName(tokens[2]), load_geometry(InputPacket(getValueClauses(clauseStrings), m_cache)));
	}
	else if(commandName == "texture")
	{
		const std::vector<std::string> clauseStrings(tokens.begin() + 3, tokens.end());
		m_cache.addTexture(getName(tokens[2]), load_texture(InputPacket(getValueClauses(clauseStrings), m_cache)));
	}
	else if(commandName == "material")
	{
		const std::vector<std::string> clauseStrings(tokens.begin() + 3, tokens.end());
		m_cache.addMaterial(getName(tokens[2]), load_material(InputPacket(getValueClauses(clauseStrings), m_cache)));
	}
	else if(commandName == "light-source")
	{
		const std::vector<std::string> clauseStrings(tokens.begin() + 3, tokens.end());
		m_cache.addLightSource(getName(tokens[2]), load_light_source(InputPacket(getValueClauses(clauseStrings), m_cache)));
	}
	else if(commandName == "actor-model")
	{
		const std::vector<std::string> clauseStrings(tokens.begin() + 3, tokens.end());
		m_cache.addActorModel(getName(tokens[2]), load_actor_model(InputPacket(getValueClauses(clauseStrings), m_cache)));
	}
	else if(commandName == "actor-light")
	{
		const std::vector<std::string> clauseStrings(tokens.begin() + 3, tokens.end());
		m_cache.addActorLight(getName(tokens[2]), load_actor_light(InputPacket(getValueClauses(clauseStrings), m_cache)));
	}
	else
	{
		std::cerr << "warning: at RenderingFileLoader::parseWorldCommand(), unknown command <" + command + ">" << std::endl;
		return;
	}
}

std::string RenderingFileParser::genName()
{
	return "@__item-" + std::to_string(m_generatedNameCounter++);
}

std::string RenderingFileParser::getName(const std::string& nameToken)
{
	if(nameToken.empty())
	{
		std::cerr << "warning: at RenderingFileLoader::getName(), empty name detected" << std::endl;
		return genName();
	}
	else if(nameToken[0] != '@')
	{
		std::cerr << "warning: at RenderingFileLoader::getName(), bad formatted name detected <" << nameToken << ">" << std::endl;
		return genName();
	}
	else if(nameToken[0] == '@' && nameToken.length() == 1)
	{
		return genName();
	}
	else
	{
		return nameToken;
	}
}

void RenderingFileParser::getCommandString(std::ifstream& dataFile, std::string* const out_command, ECommandType* const out_type)
{
	std::string lineString;
	out_command->clear();
	*out_type = ECommandType::UNKNOWN;
	while(dataFile.good())
	{
		const std::size_t position = dataFile.tellg();
		std::getline(dataFile, lineString);
		const ECommandType commandType = getCommandType(lineString);

		if(out_command->empty())
		{
			if(commandType != ECommandType::UNKNOWN)
			{
				*out_type = commandType;
				*out_command += lineString;
			}
		}
		else
		{
			if(commandType == ECommandType::UNKNOWN)
			{
				*out_command += lineString;
			}
			else
			{
				dataFile.seekg(position, std::ios_base::beg);
				break;
			}
		}
	}
}

std::vector<ValueClause> RenderingFileParser::getValueClauses(const std::vector<std::string>& clauseStrings)
{
	std::vector<ValueClause> vClauses;
	for(const auto& clauseString : clauseStrings)
	{
		vClauses.push_back(ValueClause(clauseString));
	}
	return vClauses;
}

ECommandType RenderingFileParser::getCommandType(const std::string& command)
{
	if(command.compare(0, 2, "->") == 0)
	{
		return ECommandType::WORLD;
	}
	else if(command.compare(0, 2, "##") == 0)
	{
		return ECommandType::CORE;
	}
	else
	{
		return ECommandType::UNKNOWN;
	}
}

}// end namespace ph