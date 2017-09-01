#pragma once

#include "FileIO/SDL/SdlTypeInfo.h"
#include "FileIO/SDL/ISdlResource.h"
#include "FileIO/SDL/SdlLoader.h"
#include "FileIO/SDL/SdlExecutor.h"

#include <string>
#include <functional>
#include <memory>
#include <vector>

namespace ph
{

class InputPacket;

class CommandEntry final
{
public:
	CommandEntry();

	CommandEntry& setTypeInfoFunc(const std::function<SdlTypeInfo()>& func);
	CommandEntry& setLoader(const SdlLoader& loader);
	CommandEntry& addExecutor(const SdlExecutor& executor);

	SdlTypeInfo typeInfo() const;
	/*SdlLoader   getLoader() const;
	SdlExecutor getExecutor(const std::string& name) const;*/

	std::shared_ptr<ISdlResource> load(const InputPacket& packet) const;
	ExitStatus execute(const std::shared_ptr<ISdlResource>& targetResource, 
	                   const std::string& functionName, 
	                   const InputPacket& packet) const;

private:
	std::function<SdlTypeInfo()> m_typeInfo;
	SdlLoader                    m_loader;
	std::vector<SdlExecutor>     m_executors;
};

}// end namespace ph