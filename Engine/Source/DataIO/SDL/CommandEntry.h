#pragma once

#include "DataIO/SDL/SdlTypeInfo.h"
#include "DataIO/SDL/ISdlResource.h"
#include "DataIO/SDL/SdlLoader.h"
#include "DataIO/SDL/SdlExecutor.h"
#include "Common/Logger.h"

#include <string>
#include <vector>

namespace ph
{

class InputPacket;

class CommandEntry final
{
public:
	CommandEntry();

	CommandEntry& setTypeInfo(const SdlTypeInfo& typeInfo);
	CommandEntry& setLoader(const SdlLoader& loader);
	CommandEntry& addExecutor(const SdlExecutor& executor);

	SdlTypeInfo typeInfo() const;
	SdlLoader   getLoader() const;
	SdlExecutor getExecutor(const std::string& name) const;

private:
	SdlTypeInfo              m_typeInfo;
	SdlLoader                m_loader;
	std::vector<SdlExecutor> m_executors;

private:
	static const Logger logger;
};

}// end namespace ph
