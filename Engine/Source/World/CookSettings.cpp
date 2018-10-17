#include "World/CookSettings.h"
#include "FileIO/SDL/InputPacket.h"

#include <iostream>

namespace ph
{

CookSettings::CookSettings() : 
	CookSettings(EAccelerator::BVH)
{}

CookSettings::CookSettings(const EAccelerator topLevelAccelerator)
{
	setTopLevelAccelerator(topLevelAccelerator);
}

// command interface

namespace
{
	CookSettings load_cook_settings(const InputPacket& packet)
	{
		CookSettings settings;

		const auto& topLevelAccelerator = packet.getString("top-level-accelerator", "");
		if(!topLevelAccelerator.empty())
		{
			if(topLevelAccelerator == "brute-force")
			{
				settings.setTopLevelAccelerator(EAccelerator::BRUTE_FORCE);
			}
			else if(topLevelAccelerator == "bvh")
			{
				settings.setTopLevelAccelerator(EAccelerator::BVH);
			}
			else if(topLevelAccelerator == "kd-tree")
			{
				settings.setTopLevelAccelerator(EAccelerator::KDTREE);
			}
			else if(topLevelAccelerator == "indexed-kd-tree")
			{
				settings.setTopLevelAccelerator(EAccelerator::INDEXED_KDTREE);
			}
			else
			{
				std::cerr << "warning: unknown accelerator <" + topLevelAccelerator + "> specified" << std::endl;
			}
		}

		return settings;
	}
}

SdlTypeInfo CookSettings::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_OPTION, "cook-settings");
}

void CookSettings::ciRegister(CommandRegister& cmdRegister)
{
	cmdRegister.setLoader(SdlLoader([](const InputPacket& packet)
	{
		return std::make_unique<CookSettings>(load_cook_settings(packet));
	}));
}

}// end namespace ph