#include "FileIO/DescriptionParser.h"
#include "FileIO/Description.h"
#include "FileIO/ResourceLoader.h"
#include "FileIO/FunctionExecutor.h"
#include "FileIO/RenderOption.h"
#include "FileIO/SDL/TCommandInterface.h"
#include "FileIO/SDL/CommandEntry.h"
#include "FileIO/SDL/SdlTypeInfo.h"

#include <iostream>
#include <sstream>

namespace ph
{

bool DescriptionParser::addCommandEntry(const CommandEntry& entry)
{
	if(!entry.typeInfo().isValid())
	{
		std::cerr << "warning: command entry has invalid type info: " << entry.typeInfo().toString() << ", not adding" << std::endl;
		return false;
	}

	const std::string& fullTypeName = getFullTypeName(entry.typeInfo());
	const auto& iter = NAMED_INTERFACE_MAP().find(fullTypeName);
	if(iter != NAMED_INTERFACE_MAP().end())
	{
		std::cerr << "warning: command entry of type <" << entry.typeInfo().toString() << "> is already present, not adding" << std::endl;
		return false;
	}

	NAMED_INTERFACE_MAP()[fullTypeName] = entry;
	return true;
}

CommandEntry DescriptionParser::getCommandEntry(const SdlTypeInfo& typeInfo)
{
	const std::string& fullTypeName = getFullTypeName(typeInfo);
	const auto& iter = NAMED_INTERFACE_MAP().find(fullTypeName);
	if(iter == NAMED_INTERFACE_MAP().end())
	{
		std::cerr << "warning: command entry <" << fullTypeName << "> does not exist" << std::endl;
		return CommandEntry();
	}

	return NAMED_INTERFACE_MAP()[fullTypeName];
}

std::unordered_map<std::string, CommandEntry>& DescriptionParser::NAMED_INTERFACE_MAP()
{
	static std::unordered_map<std::string, CommandEntry> namedInterfaceMap;
	return namedInterfaceMap;
}

DescriptionParser::DescriptionParser() : 
	m_commandCache(), 
	m_coreCommandTokenizer({' ', '\t', '\n', '\r'}, {{'[', ']'}}),
	m_worldCommandTokenizer({' ', '\t', '\n', '\r'}, {{'\"', '\"'}, {'[', ']'}, {'(', ')'}}), 
	m_nameTokenizer({}, {{'\"', '\"'}}),
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
	std::vector<std::string> tokens;
	m_worldCommandTokenizer.tokenize(command, tokens);

	// skip command-prefix-only command
	if(tokens.size() == 1)
	{
		return;
	}

	if(isLoadCommand(tokens))
	{
		std::cout << ">>>>>>>>>>>>>>> resource load: begin" << std::endl;

		const std::string&             categoryName = tokens[1];
		const std::string&             typeName = tokens[2];
		const std::string&             resourceName = getName(tokens[3]);
		const SdlTypeInfo              typeInfo(SdlTypeInfo::nameToCategory(categoryName), typeName);
		const std::vector<std::string> clauseStrings(tokens.begin() + 4, tokens.end());
		const InputPacket              inputPacket(getValueClauses(clauseStrings), out_data.resources);

		const auto& loadedResource = getCommandEntry(typeInfo).load(inputPacket);
		out_data.resources.addResource(typeInfo, resourceName, loadedResource);

		std::cout << "type info: " << typeInfo.toString() << std::endl;
		std::cout << "res name:  " << resourceName << std::endl;

		std::cout << ">>>>>>>>>>>>>>> resource load: end" << std::endl << std::endl;
	}
	else if(isExecuteCommand(tokens))
	{
		std::cout << ">>>>>>>>>>>>>>> execute: begin" << std::endl;

		const std::string&             categoryName = tokens[1];
		const std::string&             typeName = tokens[2];
		const std::string&             functionName = tokens[3];
		const std::string&             targetResourceName = getName(tokens[4]);
		const SdlTypeInfo              typeInfo(SdlTypeInfo::nameToCategory(categoryName), typeName);
		const std::vector<std::string> clauseStrings(tokens.begin() + 5, tokens.end());
		const InputPacket              inputPacket(getValueClauses(clauseStrings), out_data.resources);

		const DataTreatment targetResourceDT = targetResourceName.empty() ? 
			DataTreatment::OPTIONAL() : DataTreatment::REQUIRED("world command parser cannot find specified target resource");

		const auto& targetResource = out_data.resources.getResource(typeInfo, targetResourceName, targetResourceDT);
		getCommandEntry(typeInfo).execute(targetResource, functionName, inputPacket);

		std::cout << "type info:       " << typeInfo.toString() << std::endl;
		std::cout << "target res name: " << targetResourceName << std::endl;
		std::cout << "function name:   " << functionName << std::endl;

		std::cout << ">>>>>>>>>>>>>>> execute: end" << std::endl << std::endl;
	}

	//if(categoryName == "geometry")
	//{
	//	std::cout << "yo" << std::endl;
	//	/*const std::vector<std::string> clauseStrings(tokens.begin() + 3, tokens.end());
	//	resources.addGeometry(getName(tokens[2]),
	//	                      ResourceLoader::loadGeometry(InputPacket(getValueClauses(clauseStrings), resources)));*/
	//}
	/*else if(commandName == "texture")
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
	}*/
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

std::string DescriptionParser::getName(const std::string& nameToken) const
{
	std::vector<std::string> tokens;
	m_nameTokenizer.tokenize(nameToken, tokens);
	if(tokens.size() != 1)
	{
		//std::cerr << "warning: at DescriptionParser::getName(), bad number of name tokens detected <" << nameToken << ">" << std::endl;
		return "";
	}
	else if(tokens.front()[0] != '@' || tokens.front().length() == 1)
	{
		//std::cerr << "warning: at DescriptionParser::getName(), bad formatted name detected <" << nameToken << ">" << std::endl;
		return "";
	}
	else
	{
		return tokens.front();
	}

	/*if(nameToken.empty())
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
	}*/
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

bool DescriptionParser::isResourceName(const std::string& token) const
{
	return !getName(token).empty();
}

std::string DescriptionParser::getFullTypeName(const SdlTypeInfo& typeInfo)
{
	const std::string& categoryName = typeInfo.getCategoryName();
	const std::string& typeName     = typeInfo.typeName;
	return categoryName + '_' + typeName;
}

bool DescriptionParser::isLoadCommand(const std::vector<std::string>& commandTokens) const
{
	if(commandTokens.size() >= 4)
	{
		if(isResourceName(commandTokens[3]))
		{
			return true;
		}
	}

	return false;
}

bool DescriptionParser::isExecuteCommand(const std::vector<std::string>& commandTokens) const
{
	if(commandTokens.size() >= 5)
	{
		if(isResourceName(commandTokens[4]))
		{
			return true;
		}
	}

	return false;
}

}// end namespace ph