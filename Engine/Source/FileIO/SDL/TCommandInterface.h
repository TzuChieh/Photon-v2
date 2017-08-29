#pragma once

#include "FileIO/SDL/CommandEntry.h"
#include "FileIO/DescriptionParser.h"
#include "FileIO/SDL/SdlTypeInfo.h"
#include "FileIO/SDL/ISdlResource.h"
#include "FileIO/SDL/ExitStatus.h"
#include "FileIO/SDL/CommandRegister.h"

#include <string>
#include <iostream>
#include <memory>
#include <utility>
#include <type_traits>
#include <functional>

namespace ph
{

class InputPacket;

template<typename T>
void register_command_interface();

template<typename DerivedType>
class TCommandInterface : public virtual ISdlResource
{
	friend void register_command_interface<DerivedType>();

public:
	virtual ~TCommandInterface() = 0;

private:
	typedef std::function<SdlTypeInfo()>          TypeInfoFuncType;
	typedef std::function<void(CommandRegister&)> RegisterFuncType;

	static void registerInterface();
};

// template implementations:

template<typename DerivedType>
void TCommandInterface<DerivedType>::registerInterface()
{
	TypeInfoFuncType typeInfoFunc = DerivedType::ciTypeInfo;
	RegisterFuncType registerFunc = DerivedType::ciRegister;

	CommandEntry cmdEntry;
	cmdEntry.setTypeInfoFunc(typeInfoFunc);

	CommandRegister cmdRegister(cmdEntry);
	registerFunc(cmdRegister);

	if(!DescriptionParser::addCommandEntry(cmdEntry))
	{
		std::cerr << "warning: at TCommandInterface<DerivedType>::registerInterface(), " 
		          << "command registering failed for <" 
		          << typeInfoFunc().toString() 
		          << ">" << std::endl;
	}

	// DEBUG
	std::cout << "entry added: " << typeInfoFunc().toString() << std::endl;
}

template<typename DerivedType>
TCommandInterface<DerivedType>::~TCommandInterface() = default;

}// end namespace ph