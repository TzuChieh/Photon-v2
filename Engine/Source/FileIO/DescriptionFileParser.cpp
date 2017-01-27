#include "FileIO/DescriptionFileParser.h"
#include "FileIO/Description.h"
#include "FileIO/resource_loading.h"
#include "FileIO/RenderOption.h"

#include <iostream>
#include <sstream>

namespace ph
{

DescriptionFileParser::DescriptionFileParser() :
	m_coreCommandTokenizer({' ', '\t', '\n', '\r'}, {{'[', ']'}}),
	m_worldCommandTokenizer({' ', '\t', '\n', '\r'}, {{'\"', '\"'}, {'[', ']'}, {'<', '>'}}), 
	m_generatedNameCounter(0)
{

}

bool DescriptionFileParser::load(const std::string& fullFilename, Description* const out_data)
{
	std::ifstream dataFile;
	dataFile.open(fullFilename, std::ios::in);
	if(!dataFile.is_open())
	{
		std::cerr << "warning: at DescriptionFileParser::load(), failed to open file <" + fullFilename + ">" << std::endl;
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
				parseCoreCommand(commandString, *out_data);
			}
			else if(commandType == ECommandType::COMMENT)
			{
				// do nothing
			}
			else
			{
				break;
			}
		}

		populateWorldWithActors(*out_data);

		dataFile.close();
		return true;
	}
}

void DescriptionFileParser::parseCoreCommand(const std::string& command, Description& out_data)
{
	std::vector<std::string> tokens;
	m_coreCommandTokenizer.tokenize(command, tokens);
	if(tokens.size() < 3)
	{
		std::cerr << "warning: at DescriptionFileParser::parseCoreCommand(), bad formatted command <" + command + ">" << std::endl;
		return;
	}

	const std::string commandName = tokens[1];
	const std::vector<std::string> clauseStrings(tokens.begin() + 2, tokens.end());
	if(commandName == "camera")
	{
		out_data.camera = load_camera(InputPacket(getValueClauses(clauseStrings), m_cache));
	}
	else if(commandName == "film")
	{
		out_data.film = load_film(InputPacket(getValueClauses(clauseStrings), m_cache));
	}
	else if(commandName == "sampler")
	{
		out_data.sampleGenerator = load_sample_generator(InputPacket(getValueClauses(clauseStrings), m_cache));
	}
	else if(commandName == "integrator")
	{
		out_data.integrator = load_integrator(InputPacket(getValueClauses(clauseStrings), m_cache));
	}
	else if(commandName == "renderer")
	{
		out_data.renderOption = RenderOption(InputPacket(getValueClauses(clauseStrings), m_cache));
	}
	else
	{
		std::cerr << "warning: at DescriptionFileParser::parseCoreCommand(), unknown command <" + command + ">" << std::endl;
		return;
	}
}

void DescriptionFileParser::parseWorldCommand(const std::string& command)
{
	std::vector<std::string> tokens;
	m_worldCommandTokenizer.tokenize(command, tokens);
	if(tokens.size() < 3)
	{
		std::cerr << "warning: at DescriptionFileParser::parseWorldCommand(), bad formatted command <" + command + ">" << std::endl;
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
	else if(commandName == "transform")
	{
		const std::vector<std::string> clauseStrings(tokens.begin() + 2, tokens.end());
	}
	else
	{
		std::cerr << "warning: at DescriptionFileParser::parseWorldCommand(), unknown command <" + command + ">" << std::endl;
		return;
	}
}

void DescriptionFileParser::populateWorldWithActors(Description& out_data)
{
	std::vector<std::unique_ptr<Actor>> actors = m_cache.claimAllActors();
	for(auto& actor : actors)
	{
		out_data.world.addActor(std::move(actor));
	}
}

std::string DescriptionFileParser::genName()
{
	return "@__item-" + std::to_string(m_generatedNameCounter++);
}

std::string DescriptionFileParser::getName(const std::string& nameToken)
{
	if(nameToken.empty())
	{
		std::cerr << "warning: at DescriptionFileParser::getName(), empty name detected" << std::endl;
		return genName();
	}
	else if(nameToken[0] != '@')
	{
		std::cerr << "warning: at DescriptionFileParser::getName(), bad formatted name detected <" << nameToken << ">" << std::endl;
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

void DescriptionFileParser::getCommandString(std::ifstream& dataFile, std::string* const out_command, ECommandType* const out_type)
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

std::vector<ValueClause> DescriptionFileParser::getValueClauses(const std::vector<std::string>& clauseStrings)
{
	std::vector<ValueClause> vClauses;
	for(const auto& clauseString : clauseStrings)
	{
		vClauses.push_back(ValueClause(clauseString));
	}
	return vClauses;
}

ECommandType DescriptionFileParser::getCommandType(const std::string& command)
{
	if(command.compare(0, 2, "->") == 0)
	{
		return ECommandType::WORLD;
	}
	else if(command.compare(0, 2, "##") == 0)
	{
		return ECommandType::CORE;
	}
	else if(command.compare(0, 2, "//") == 0)
	{
		return ECommandType::COMMENT;
	}
	else
	{
		return ECommandType::UNKNOWN;
	}
}

}// end namespace ph