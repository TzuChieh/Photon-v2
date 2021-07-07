#pragma once

#include "DataIO/Option.h"
#include "DataIO/SDL/sdl_interface.h"

#include <string>

namespace ph
{

class EngineOption : public Option
{
public:
	std::string getRendererName() const;
	std::string getReceiverName() const;
	std::string getSampleGeneratorName() const;
	std::string getCookSettingsName() const;

private:
	std::string m_rendererName;
	std::string m_receiverName;
	std::string m_sampleGeneratorName;
	std::string m_cookSettingsName;

public:
	PH_DEFINE_SDL_CLASS(TOwnerSdlClass<EngineOption>)
	{
		ClassType clazz("engine");
		clazz.description("Settings for the core engine execution.");
		clazz.baseOn<Option>();

		TSdlString<OwnerType> rendererName("renderer", &OwnerType::m_rendererName);
		rendererName.description("Name of the renderer resource to use.");
		rendererName.optional();
		clazz.addField(rendererName);

		TSdlString<OwnerType> receiverName("receiver", &OwnerType::m_receiverName);
		receiverName.description("Name of the receiver resource to use.");
		receiverName.optional();
		clazz.addField(receiverName);

		TSdlString<OwnerType> sampleGeneratorName("sample-generator", &OwnerType::m_sampleGeneratorName);
		sampleGeneratorName.description("Name of the sample generator resource to use.");
		sampleGeneratorName.optional();
		clazz.addField(sampleGeneratorName);

		TSdlString<OwnerType> cookSettingsName("cook-settings", &OwnerType::m_cookSettingsName);
		cookSettingsName.description("Name of the cook settings resource to use.");
		cookSettingsName.optional();
		clazz.addField(cookSettingsName);

		return clazz;
	}
};

// In-header Implementations:

inline std::string EngineOption::getRendererName() const
{
	return m_rendererName;
}

inline std::string EngineOption::getReceiverName() const
{
	return m_receiverName;
}

inline std::string EngineOption::getSampleGeneratorName() const
{
	return m_sampleGeneratorName;
}

inline std::string EngineOption::getCookSettingsName() const
{
	return m_cookSettingsName;
}

}// end namespace ph
