#pragma once

#include "FileIO/InputPacket.h"
#include "FileIO/InputPrototype.h"
#include "FileIO/SDL/CommandEntry.h"
#include "FileIO/DescriptionParser.h"

#include <string>
#include <iostream>
#include <memory>

namespace ph
{

template<typename T>
void register_command_interface();

template<typename DerivedType>
class TCommandInterface
{
protected:
	static std::string typeNameImpl();
	static void executeImpl(const std::string& functionName, const InputPacket& packet);

private:
	friend void register_command_interface<DerivedType>();
	static void registerInterface();

	static std::string typeName();
	static std::shared_ptr<void> load(const InputPacket& packet);
	static void execute(const std::string& functionName, const InputPacket& packet);
};

// Implementations:

template<typename DerivedType>
void TCommandInterface<DerivedType>::registerInterface()
{
	CommandEntry entry;
	entry.setTypeNameFunc(typeName);
	entry.setLoadFunc(load);
	entry.setExecuteFunc(execute);

	if(!DescriptionParser::addCommandEntry(entry))
	{
		std::cerr << "warning: command registering failed" << std::endl;
	}
}

template<typename DerivedType>
std::string TCommandInterface<DerivedType>::typeName()
{
	return DerivedType::typeNameImpl();
}

template<typename DerivedType>
void TCommandInterface<DerivedType>::execute(const std::string& functionName, const InputPacket& packet)
{
	DerivedType::executeImpl(functionName, packet);
}

template<typename DerivedType>
std::shared_ptr<void> TCommandInterface<DerivedType>::load(const InputPacket& packet)
{
	return std::make_shared<DerivedType>(packet);
}

template<typename DerivedType>
std::string TCommandInterface<DerivedType>::typeNameImpl()
{
	std::cerr << "CommandInterface Warning: using unspecified type name" << std::endl;
	return "";
}

template<typename DerivedType>
void TCommandInterface<DerivedType>::executeImpl(const std::string& functionName, const InputPacket& packet)
{
	std::cerr << "CommandInterface Warning: function <" << functionName << "> not found" << std::endl;
}

}// end namespace ph