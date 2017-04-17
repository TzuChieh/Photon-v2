#include "FileIO/DescriptionParser.h"
#include "FileIO/Description.h"
#include "FileIO/ResourceLoader.h"
#include "FileIO/FunctionExecutor.h"
#include "FileIO/RenderOption.h"
#include "FileIO/SDL/TCommandInterface.h"
#include "FileIO/SDL/CommandEntry.h"

#include <iostream>
#include <sstream>

namespace ph
{

bool DescriptionParser::addCommandEntry(const CommandEntry& entry)
{
	const auto& iter = NAMED_INTERFACE_MAP().find(entry.typeName());
	if(iter != NAMED_INTERFACE_MAP().end())
	{
		std::cerr << "warning: command entry <" << entry.typeName() << "> is already present, not adding" << std::endl;
		return false;
	}

	NAMED_INTERFACE_MAP()[entry.typeName()] = entry;
	return true;
}

std::unordered_map<std::string, CommandEntry>& DescriptionParser::NAMED_INTERFACE_MAP()
{
	static std::unordered_map<std::string, CommandEntry> namedInterfaceMap;
	return namedInterfaceMap;
}

DescriptionParser::DescriptionParser() : 
	m_commandCache(), 
	m_coreCommandTokenizer({' ', '\t', '\n', '\r'}, {{'[', ']'}}),
	m_worldCommandTokenizer({' ', '\t', '\n', '\r'}, {{'\"', '\"'}, {'[', ']'}, {'<', '>'}}), 
	m_generatedNameCounter(0)
{

}

void DescriptionParser::enter(const std::string& commandFragment, Description& out_data)
{
	if(getCommandType(commandFragment) != ECommandType::UNKNOWN)
	{
		parseCommand(m_commandCache, out_data);

		m_commandCache.clear();
		m_commandCache.shrink_to_fit();
	}

	m_commandCache += commandFragment;
}

void DescriptionParser::parseCommand(const std::string& command, Description& out_data)
{
	if(command.empty())
	{
		return;
	}

	const ECommandType commandType = getCommandType(command);

	if(commandType == ECommandType::WORLD)
	{
		parseWorldCommand(command, out_data);
	}
	else if(commandType == ECommandType::CORE)
	{
		parseCoreCommand(command, out_data);
	}
	else if(commandType == ECommandType::COMMENT)
	{
		// do nothing
	}
	else
	{
		std::cerr << "warning: at DescriptionParser::parseCachedCommand(), unsupported command type detected" << std::endl;
	}
}

void DescriptionParser::parseCoreCommand(const std::string& command, Description& out_data)
{
	auto& resources = out_data.resources;

	std::vector<std::string> tokens;
	m_coreCommandTokenizer.tokenize(command, tokens);

	if(tokens.size() == 1)
	{
		return;
	}

	if(tokens.size() < 2)
	{
		std::cerr << "warning: at DescriptionParser::parseCoreCommand(), bad formatted command <" + command + ">" << std::endl;
		return;
	}

	const std::string commandName = tokens[1];
	if(commandName == "camera")
	{
		const std::vector<std::string> clauseStrings(tokens.begin() + 2, tokens.end());
		out_data.camera = ResourceLoader::loadCamera(InputPacket(getValueClauses(clauseStrings), resources));
	}
	else if(commandName == "film")
	{
		const std::vector<std::string> clauseStrings(tokens.begin() + 2, tokens.end());
		out_data.film = ResourceLoader::loadFilm(InputPacket(getValueClauses(clauseStrings), resources));
	}
	else if(commandName == "sampler")
	{
		const std::vector<std::string> clauseStrings(tokens.begin() + 2, tokens.end());
		out_data.sampleGenerator = ResourceLoader::loadSampleGenerator(InputPacket(getValueClauses(clauseStrings), resources));
	}
	else if(commandName == "integrator")
	{
		const std::vector<std::string> clauseStrings(tokens.begin() + 2, tokens.end());
		out_data.integrator = ResourceLoader::loadIntegrator(InputPacket(getValueClauses(clauseStrings), resources));
	}
	else if(commandName == "renderer")
	{
		const std::vector<std::string> clauseStrings(tokens.begin() + 2, tokens.end());
		out_data.renderOption = RenderOption(InputPacket(getValueClauses(clauseStrings), resources));
	}
	/*else if(commandName == "render")
	{
		populateWorldWithActors(out_data);
	}*/
	else
	{
		std::cerr << "warning: at DescriptionParser::parseCoreCommand(), unknown command <" + command + ">" << std::endl;
		return;
	}
}

void DescriptionParser::parseWorldCommand(const std::string& command, Description& out_data)
{
	auto& resources = out_data.resources;

	std::vector<std::string> tokens;
	m_worldCommandTokenizer.tokenize(command, tokens);

	if(tokens.size() == 1)
	{
		return;
	}

	if(tokens.size() < 3)
	{
		std::cerr << "warning: at DescriptionParser::parseWorldCommand(), bad formatted command <" + command + ">" << std::endl;
		return;
	}

	const std::string commandName = tokens[1];
	if(commandName == "geometry")
	{
		const std::vector<std::string> clauseStrings(tokens.begin() + 3, tokens.end());
		resources.addGeometry(getName(tokens[2]),
		                      ResourceLoader::loadGeometry(InputPacket(getValueClauses(clauseStrings), resources)));
	}
	else if(commandName == "texture")
	{
		const std::vector<std::string> clauseStrings(tokens.begin() + 3, tokens.end());
		resources.addTexture(getName(tokens[2]),
		                     ResourceLoader::loadTexture(InputPacket(getValueClauses(clauseStrings), resources)));
	}
	else if(commandName == "material")
	{
		const std::vector<std::string> clauseStrings(tokens.begin() + 3, tokens.end());
		resources.addMaterial(getName(tokens[2]),
		                      ResourceLoader::loadMaterial(InputPacket(getValueClauses(clauseStrings), resources)));
	}
	else if(commandName == "light-source")
	{
		const std::vector<std::string> clauseStrings(tokens.begin() + 3, tokens.end());
		resources.addLightSource(getName(tokens[2]),
		                         ResourceLoader::loadLightSource(InputPacket(getValueClauses(clauseStrings), resources)));
	}
	else if(commandName == "actor-model")
	{
		const std::vector<std::string> clauseStrings(tokens.begin() + 3, tokens.end());
		resources.addActorModel(getName(tokens[2]),
		                        ResourceLoader::loadActorModel(InputPacket(getValueClauses(clauseStrings), resources)));
	}
	else if(commandName == "actor-light")
	{
		const std::vector<std::string> clauseStrings(tokens.begin() + 3, tokens.end());
		resources.addActorLight(getName(tokens[2]),
		                        ResourceLoader::loadActorLight(InputPacket(getValueClauses(clauseStrings), resources)));
	}
	else if(commandName == "transform")
	{
		const std::vector<std::string> clauseStrings(tokens.begin() + 2, tokens.end());
		FunctionExecutor::executeTransform(InputPacket(getValueClauses(clauseStrings), resources));
	}
	else
	{
		std::cerr << "warning: at DescriptionParser::parseWorldCommand(), unknown command <" + command + ">" << std::endl;
		return;
	}
}

//void DescriptionParser::populateWorldWithActors(Description& out_data)
//{
//	std::vector<std::unique_ptr<Actor>> actors = m_resourceCache.claimAllActors();
//	for(auto& actor : actors)
//	{
//		out_data.visualWorld.addActor(std::move(actor));
//	}
//
//	//std::cerr << "warning: DescriptionParser::populateWorldWithActors() not implemented" << std::endl;
//}

std::string DescriptionParser::genName()
{
	return "@__item-" + std::to_string(m_generatedNameCounter++);
}

std::string DescriptionParser::getName(const std::string& nameToken)
{
	if(nameToken.empty())
	{
		std::cerr << "warning: at DescriptionParser::getName(), empty name detected" << std::endl;
		return genName();
	}
	else if(nameToken[0] != '@')
	{
		std::cerr << "warning: at DescriptionParser::getName(), bad formatted name detected <" << nameToken << ">" << std::endl;
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

void DescriptionParser::getCommandString(std::ifstream& dataFile, std::string* const out_command, ECommandType* const out_type)
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

std::vector<ValueClause> DescriptionParser::getValueClauses(const std::vector<std::string>& clauseStrings)
{
	std::vector<ValueClause> vClauses;
	for(const auto& clauseString : clauseStrings)
	{
		vClauses.push_back(ValueClause(clauseString));
	}
	return vClauses;
}

ECommandType DescriptionParser::getCommandType(const std::string& command)
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