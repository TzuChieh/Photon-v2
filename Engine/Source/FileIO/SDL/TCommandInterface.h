#pragma once

#include "FileIO/SDL/CommandEntry.h"
#include "FileIO/DescriptionParser.h"
#include "FileIO/SDL/SdlTypeInfo.h"
#include "FileIO/SDL/ISdlResource.h"
#include "FileIO/SDL/ExitStatus.h"

#include <string>
#include <iostream>
#include <memory>
#include <utility>
#include <type_traits>

namespace ph
{

class InputPacket;

template<typename T>
void register_command_interface();

template<typename DerivedType>
class TCommandInterface
{
	friend void register_command_interface<DerivedType>();

private:
	static void registerInterface();
	static SdlTypeInfo typeInfo();
	static void execute(const std::shared_ptr<ISdlResource>& targetResource, const std::string& functionName, const InputPacket& packet);
	static std::shared_ptr<ISdlResource> load(const InputPacket& packet);

	// forward load() to SFINAE-determined static methods

	template<typename = std::enable_if_t<!std::is_abstract<DerivedType>::value>>
	static std::shared_ptr<ISdlResource> conditionalLoad(const InputPacket& packet);

	template<typename = std::enable_if_t<std::is_abstract<DerivedType>::value>>
	static std::shared_ptr<DerivedType> conditionalLoad(const InputPacket& packet);
};

// template implementations:

template<typename DerivedType>
void TCommandInterface<DerivedType>::registerInterface()
{
	CommandEntry entry;
	entry.setTypeInfoFunc(typeInfo);
	entry.setLoadFunc(load);
	entry.setExecuteFunc(execute);

	if(!DescriptionParser::addCommandEntry(entry))
	{
		std::cerr << "warning: command registering failed" << std::endl;
	}

	// DEBUG
	std::cout << "entry added: " << typeInfo().toString() << std::endl;
}

template<typename DerivedType>
SdlTypeInfo TCommandInterface<DerivedType>::typeInfo()
{
	return DerivedType::ciTypeInfo();
}

// input target resource is allowed to be null since execute() may not necessarily operate on resources
template<typename DerivedType>
void TCommandInterface<DerivedType>::execute(const std::shared_ptr<ISdlResource>& targetResource, const std::string& functionName, const InputPacket& packet)
{
	const std::shared_ptr<DerivedType> castedTargetResource = std::dynamic_pointer_cast<DerivedType>(targetResource);
	if(targetResource != nullptr && castedTargetResource == nullptr)
	{
		std::cerr << "warning: type <" << typeInfo().toString() << ">'s function <" << functionName 
		          << "> cannot operate on specified type (casting failed)" << std::endl;
		return;
	}

	const ExitStatus& executionExitStatus = DerivedType::ciExecute(castedTargetResource, functionName, packet);
	const std::string& functionInfo = "type <" + typeInfo().toString() + ">'s function <" + functionName + ">";
	switch(executionExitStatus.state)
	{
	case ExitStatus::State::SUCCESS:
		if(!executionExitStatus.message.empty())
		{
			std::cout << functionInfo << " successfully executed" << std::endl;
			std::cout << executionExitStatus.message << std::endl;
		}
		break;

	case ExitStatus::State::WARNING:
		std::cerr << functionInfo << " executed, but with warning" << std::endl;
		std::cerr << executionExitStatus.message << std::endl;
		break;

	case ExitStatus::State::FAILURE:
		std::cerr << functionInfo << " executed and failed" << std::endl;
		std::cerr << executionExitStatus.message << std::endl;
		break;

	case ExitStatus::State::BAD_INPUT:
		std::cerr << functionInfo << " ignored because of bad input" << std::endl;
		std::cerr << executionExitStatus.message << std::endl;
		break;

	case ExitStatus::State::UNSUPPORTED:
		std::cerr << "calling unsupported function: " << functionInfo << std::endl;
		if(!executionExitStatus.message.empty())
		{
			std::cerr << executionExitStatus.message << std::endl;
		}
		break;
	}
}

template<typename DerivedType>
std::shared_ptr<ISdlResource> TCommandInterface<DerivedType>::load(const InputPacket& packet)
{
	return conditionalLoad(packet);
}

template<typename DerivedType>
template<typename>
static std::shared_ptr<ISdlResource> TCommandInterface<DerivedType>::conditionalLoad(const InputPacket& packet)
{
	return std::make_shared<DerivedType>(packet);
}

template<typename DerivedType>
template<typename>
static std::shared_ptr<DerivedType> TCommandInterface<DerivedType>::conditionalLoad(const InputPacket& packet)
{
	std::cerr << "warning: cannot load abstract class <" << typeInfo().toString() << ">, returning nullptr" << std::endl;
	return nullptr;
}

}// end namespace ph