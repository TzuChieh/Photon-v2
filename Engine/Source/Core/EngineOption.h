#pragma once

#include "DataIO/Option.h"

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
