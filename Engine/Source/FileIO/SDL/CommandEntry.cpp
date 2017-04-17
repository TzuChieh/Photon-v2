#include "FileIO/SDL/CommandEntry.h"

#include <iostream>

namespace ph
{

CommandEntry::CommandEntry() : 
	m_typeName(nullptr), m_load(nullptr), m_execute(nullptr)
{

}

CommandEntry& CommandEntry::setTypeNameFunc(const TypeNameFuncType& func)
{
	m_typeName = func;
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

std::string CommandEntry::typeName() const
{
	if(m_typeName == nullptr)
	{
		std::cerr << "warning: at CommandEntry::typeName(), empty function pointer" << std::endl;
		return "";
	}

	return m_typeName();
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