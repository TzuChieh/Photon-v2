#pragma once

#include "FileIO/InputPacket.h"
#include "FileIO/InputPrototype.h"
#include "FileIO/SDL/CommandEntry.h"
#include "FileIO/DescriptionParser.h"
#include "FileIO/SDL/SdlTypeInfo.h"

#include <string>
#include <iostream>
#include <memory>
#include <utility>

namespace ph
{

template<typename T>
void register_command_interface();

template<typename DerivedType>
class TCommandInterface
{
private:
	friend void register_command_interface<DerivedType>();
	static void registerInterface();

	static SdlTypeInfo typeInfo();
	static std::shared_ptr<void> load(const InputPacket& packet);
	static void execute(const std::string& functionName, const InputPacket& packet);
};

// Implementations:

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
	return DerivedType::iTypeInfoImpl();
}

template<typename DerivedType>
void TCommandInterface<DerivedType>::execute(const std::string& functionName, const InputPacket& packet)
{
	DerivedType::iExecuteImpl(functionName, packet);
}

template<typename DerivedType>
std::shared_ptr<void> TCommandInterface<DerivedType>::load(const InputPacket& packet)
{
	return std::make_shared<DerivedType>(packet);
}

}// end namespace ph