#include "Core/EngineOption.h"
#include "DataIO/SDL/SdlLoader.h"
#include "DataIO/SDL/InputPacket.h"

namespace ph
{

// command interface

EngineOption::EngineOption(const InputPacket& packet) : 

	Option(packet),

	m_rendererName       (packet.getString("renderer")),
	m_receiverName       (packet.getString("receiver")),
	m_sampleGeneratorName(packet.getString("sample-generator")),
	m_cookSettingsName   (packet.getString("cook-settings"))
{}

SdlTypeInfo EngineOption::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_OPTION, "engine");
}

void EngineOption::ciRegister(CommandRegister& cmdRegister)
{
	cmdRegister.setLoader(
		SdlLoader([](const InputPacket& packet)
		{
			return std::make_unique<EngineOption>(packet);
		}));
}

}// end namespace ph
