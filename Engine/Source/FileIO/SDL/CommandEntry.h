#pragma once

#include "FileIO/InputPacket.h"

#include <string>
#include <functional>
#include <memory>

namespace ph
{

class CommandEntry final
{
public:
	typedef std::function<std::string()>                                                    TypeNameFuncType;
	typedef std::function<std::shared_ptr<void>(const InputPacket& packet)>                 LoadFuncType;
	typedef std::function<void(const std::string& functionName, const InputPacket& packet)> ExecuteFuncType;

	CommandEntry();

	CommandEntry& setTypeNameFunc(const TypeNameFuncType& func);
	CommandEntry& setLoadFunc(const LoadFuncType& func);
	CommandEntry& setExecuteFunc(const ExecuteFuncType& func);

	std::string typeName() const;
	std::shared_ptr<void> load(const InputPacket& packet) const;
	void execute(const std::string& functionName, const InputPacket& packet) const;

private:
	TypeNameFuncType m_typeName;
	LoadFuncType     m_load;
	ExecuteFuncType  m_execute;
};

}// end namespace ph