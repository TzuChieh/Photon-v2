#include "FileIO/SDL/CommandEntry.h"

#include <iostream>

namespace ph
{

CommandEntry::CommandEntry() : 
	m_typeInfo(nullptr), m_load(nullptr), m_execute(nullptr)
{

}

CommandEntry& CommandEntry::setTypeInfoFunc(const TypeInfoFuncType& func)
{
	m_typeInfo = func;
	return *this;
}

CommandEntry& CommandEntry::setLoadFunc(const LoadFuncType& func)
{
	m_load = func;
	return *this;
}

CommandEntry& CommandEntry::setExecuteFunc(const ExecuteFuncType& func)
{
	m_execute = func;
	return *this;
}

SdlTypeInfo CommandEntry::typeInfo() const
{
	if(m_typeInfo == nullptr)
	{
		std::cerr << "warning: at CommandEntry::typeInfo(), empty function pointer" << std::endl;
		return SdlTypeInfo::makeInvalid();
	}

	return m_typeInfo();
}

std::shared_ptr<void> CommandEntry::load(const InputPacket& packet) const
{
	if(m_load == nullptr)
	{
		std::cerr << "warning: at CommandEntry::load(), empty function pointer" << std::endl;
		return nullptr;
	}

	return m_load(packet);
}

void CommandEntry::execute(const std::string& functionName, const InputPacket& packet) const
{
	if(m_execute == nullptr)
	{
		std::cerr << "warning: at CommandEntry::execute(), empty function pointer" << std::endl;
		return;
	}

	m_execute(functionName, packet);
}

}// end namespace ph