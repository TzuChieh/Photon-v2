#pragma once

#include "FileIO/SDL/SdlTypeInfo.h"
#include "FileIO/SDL/ISdlResource.h"

#include <string>
#include <functional>
#include <memory>

namespace ph
{

class InputPacket;

class CommandEntry final
{
public:
	typedef std::function<SdlTypeInfo()>                                                    TypeInfoFuncType;
	typedef std::function<std::shared_ptr<ISdlResource>(const InputPacket& packet)>         LoadFuncType;
	typedef std::function<void(const std::shared_ptr<ISdlResource>& targetResource, 
	                           const std::string& functionName, 
	                           const InputPacket& packet)>                                  ExecuteFuncType;

	CommandEntry();

	CommandEntry& setTypeInfoFunc(const TypeInfoFuncType& func);
	CommandEntry& setLoadFunc(const LoadFuncType& func);
	CommandEntry& setExecuteFunc(const ExecuteFuncType& func);

	SdlTypeInfo typeInfo() const;
	std::shared_ptr<ISdlResource> load(const InputPacket& packet) const;
	void execute(const std::shared_ptr<ISdlResource>& targetResource, const std::string& functionName, const InputPacket& packet) const;

private:
	TypeInfoFuncType m_typeInfo;
	LoadFuncType     m_load;
	ExecuteFuncType  m_execute;
};

}// end namespace ph