#pragma once

#include "DataIO/SDL/CommandEntry.h"
#include "DataIO/SDL/SdlParser.h"
#include "DataIO/SDL/SdlTypeInfo.h"
#include "DataIO/SDL/ISdlResource.h"
#include "DataIO/SDL/ExitStatus.h"
#include "DataIO/SDL/CommandRegister.h"

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
	virtual ~TCommandInterface() = default;

private:
	using TypeInfoFuncType = std::function<SdlTypeInfo()>;
	using RegisterFuncType = std::function<void(CommandRegister&)>;

	static void registerInterface();
};

// template implementations:

template<typename DerivedType>
inline void TCommandInterface<DerivedType>::registerInterface()
{
	// TODO: check for method signature

	TypeInfoFuncType typeInfoFunc = DerivedType::ciTypeInfo;
	RegisterFuncType registerFunc = DerivedType::ciRegister;

	CommandEntry cmdEntry;
	cmdEntry.setTypeInfo(typeInfoFunc());

	CommandRegister cmdRegister(cmdEntry);
	registerFunc(cmdRegister);

	if(!SdlParser::addCommandEntry(cmdEntry))
	{
		std::cerr << "warning: at TCommandInterface<DerivedType>::registerInterface(), " 
		          << "command registering failed for <" 
		          << typeInfoFunc().toString() 
		          << ">" << std::endl;
		return;
	}
}

}// end namespace ph
