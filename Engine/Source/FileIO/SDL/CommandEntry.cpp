#include "FileIO/SDL/CommandEntry.h"

#include <iostream>

namespace ph
{

CommandEntry::CommandEntry() : 
	m_typeInfo(nullptr), m_loader(), m_executors()
{

}

CommandEntry& CommandEntry::setTypeInfoFunc(const std::function<SdlTypeInfo()>& func)
{
	m_typeInfo = func;

	return *this;
}

CommandEntry& CommandEntry::setLoader(const SdlLoader& loader)
{
	m_loader = loader;

	return *this;
}

CommandEntry& CommandEntry::addExecutor(const SdlExecutor& executor)
{
	m_executors.push_back(executor);

	return *this;
}

SdlTypeInfo CommandEntry::typeInfo() const
{
	if(m_typeInfo == nullptr)
	{
		std::cerr << "warning: at CommandEntry::typeInfo(), " 
		          << "no type info provided" << std::endl;
		return SdlTypeInfo::makeInvalid();
	}

	return m_typeInfo();
}

//SdlLoader CommandEntry::getLoader() const
//{
//
//}
//
//SdlExecutor CommandEntry::getExecutor(const std::string& name) const
//{
//
//}

std::shared_ptr<ISdlResource> CommandEntry::load(const InputPacket& packet) const
{
	std::shared_ptr<ISdlResource> res = m_loader.load(packet);

	if(res == nullptr)
	{
		std::cerr << "warning: at CommandEntry::load(), " 
		          << "loading failed for type <" 
		          << typeInfo().toString() 
		          << "> due to no loader provided or some internal error" << std::endl;
	}

	return res;
}

ExitStatus CommandEntry::execute(const std::shared_ptr<ISdlResource>& targetResource, 
                                 const std::string& functionName, 
                                 const InputPacket& packet) const
{
	const SdlExecutor* targetExecutor = nullptr;
	for(const auto& executor : m_executors)
	{
		if(executor.getName() == functionName)
		{
			targetExecutor = &executor;
			break;
		}
	}

	if(targetExecutor == nullptr)
	{
		return ExitStatus::FAILURE(std::string()
			+ "warning: at CommandEntry::execute(), " 
			+ "no valid executor named <"
			+ functionName
			+ "> provided by <"
			+ typeInfo().toString()
			+ ">");
	}

	return targetExecutor->execute(targetResource, packet);
}

}// end namespace ph