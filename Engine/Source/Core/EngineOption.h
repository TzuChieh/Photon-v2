#pragma once

#include "DataIO/Option.h"
#include "DataIO/SDL/TCommandInterface.h"

#include <string>

namespace ph
{

class EngineOption : public Option, public TCommandInterface<EngineOption>
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

// command interface
public:
	explicit EngineOption(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
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

/*
	<SDL_interface>

	<category>  option </category>
	<type_name> engine </type_name>

	<name> Engine Option </name>
	<description>
		Settings related to the render engine.
	</description>

	<command type="creator">

		<input name="renderer" type="string">
			<description>
				Name of the renderer resource to use.
			</description>
		</input>

		<input name="receiver" type="string">
			<description>
				Name of the receiver resource to use.
			</description>
		</input>

		<input name="sample-generator" type="string">
			<description>
				Name of the sample generator resource to use.
			</description>
		</input>

	</command>

	</SDL_interface>
*/
