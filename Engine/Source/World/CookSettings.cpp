#include "World/CookSettings.h"
#include "DataIO/SDL/InputPacket.h"
#include "Common/Logger.h"

#include <iostream>

namespace ph
{

namespace
{
	Logger logger(LogSender("Cook Settings"));
}

CookSettings::CookSettings() : 
	CookSettings(EAccelerator::BVH)
{}

CookSettings::CookSettings(const EAccelerator topLevelAccelerator)
{
	setTopLevelAccelerator(topLevelAccelerator);
}

// command interface

CookSettings::CookSettings(const InputPacket& packet) : 

	Option(packet),

	m_topLevelAccelerator(EAccelerator::BVH)
{
	const auto& topLevelAccelerator = packet.getString("top-level-accelerator", "bvh");
	if(topLevelAccelerator == "brute-force")
	{
		setTopLevelAccelerator(EAccelerator::BRUTE_FORCE);
	}
	else if(topLevelAccelerator == "bvh")
	{
		setTopLevelAccelerator(EAccelerator::BVH);
	}
	else if(topLevelAccelerator == "kd-tree")
	{
		setTopLevelAccelerator(EAccelerator::KDTREE);
	}
	else if(topLevelAccelerator == "indexed-kd-tree")
	{
		setTopLevelAccelerator(EAccelerator::INDEXED_KDTREE);
	}
	else
	{
		logger.log(ELogLevel::NOTE_MED,
			"unknown accelerator <" + topLevelAccelerator + "> specified");
	}
}

SdlTypeInfo CookSettings::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_OPTION, "cook-settings");
}

void CookSettings::ciRegister(CommandRegister& cmdRegister)
{
	cmdRegister.setLoader(
		SdlLoader([](const InputPacket& packet)
		{
			return std::make_unique<CookSettings>(packet);
		}));
}

}// end namespace ph
