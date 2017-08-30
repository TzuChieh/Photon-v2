#pragma once

#include "FileIO/SDL/CommandEntry.h"
#include "FileIO/SDL/SdlExecutor.h"
#include "FileIO/SDL/SdlLoader.h"

namespace ph
{

class CommandRegister final
{
public:
	inline CommandRegister(CommandEntry& entry) : 
		m_entry(entry)
	{

	}

	inline void setLoader(const SdlLoader& loader)
	{
		m_entry.setLoader(loader);
	}

	inline void addExecutor(const SdlExecutor& executor)
	{
		m_entry.addExecutor(executor);
	}

	template<typename OwnerType, typename TargetType>
	inline void reintroduce()
	{
		CommandEntry    collectorEntry;
		CommandRegister collectorRegister(collectorEntry);

		TargetType::ciRegister(collectorRegister);
	}

private:
	CommandEntry& m_entry;
};

}// end namespace ph