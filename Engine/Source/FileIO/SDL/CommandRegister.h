#pragma once

#include "FileIO/SDL/CommandEntry.h"
#include "FileIO/SDL/SdlExecutor.h"
#include "FileIO/SDL/SdlLoader.h"

#include <type_traits>

namespace ph
{

class CommandRegister final
{
public:
	inline CommandRegister(CommandEntry& entry) : 
		m_entry(entry)
	{}

	inline void setLoader(const SdlLoader& loader)
	{
		m_entry.setLoader(loader);
	}

	inline void addExecutor(const SdlExecutor& executor)
	{
		m_entry.addExecutor(executor);
	}

private:
	CommandEntry& m_entry;
};

}// end namespace ph
