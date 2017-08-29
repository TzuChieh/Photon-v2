#pragma once

#include "FileIO/SDL/CommandEntry.h"
#include "FileIO/SDL/ExitStatus.h"
#include "FileIO/SDL/SdlTypeInfo.h"

#include <string>
#include <functional>
#include <memory>

namespace ph
{

class InputPacket;

class SdlExecutor final
{
public:
	inline SdlExecutor() : 
		m_targetTypeInfo(SdlTypeInfo::makeInvalid()), 
		m_name(), 
		m_func(nullptr)
	{

	}

	inline void setName(const std::string& name)
	{
		m_name = name;
	}

	template<typename T>
	inline void setFunc(
		const std::function
		<
			ExitStatus(const std::shared_ptr<T>& targetResource,
			           const InputPacket& packet)
		>& func)
	{
		m_targetTypeInfo = T::ciTypeInfo();

		m_func = [this, func](const std::shared_ptr<ISdlResource>& res,
		                      const InputPacket& pac) -> ExitStatus
		{
			// input target resource is allowed to be null since executor may not 
			// necessarily operate on resources, so we only check for situations
			// where input target resource is non-null
			const std::shared_ptr<T> castedRes = std::dynamic_pointer_cast<T>(res);
			if(res != nullptr && castedRes == nullptr)
			{
				return ExitStatus::FAILURE(std::string() +
					"warning: at SdlExecutor, " 
					+ "executor <"
					+ m_name
					+ "> accepts only resources of type <"
					+ m_targetTypeInfo.toString()
					+ "> (casting failed)");
			}

			return func(castedRes, pac);
		}
	}

	inline ExitStatus execute(
		const std::shared_ptr<ISdlResource>& targetResource,
		const InputPacket& packet) const
	{
		if(m_func == nullptr)
		{
			return ExitStatus::FAILURE(std::string() + 
				"warning: at SdlExecutor, "
				+ "executor <"
				+ m_name
				+ "> is empty");
		}

		return m_func(targetResource, packet);
	}

	inline std::string getName() const
	{
		return m_name;
	}

	inline bool isValid() const
	{
		return m_targetTypeInfo.isValid() && !m_name.empty() && m_func != nullptr;
	}

private:
	typedef std::function
	<
		ExitStatus(const std::shared_ptr<ISdlResource>& targetResource,
		           const InputPacket& packet)
	> ExecuteFuncType;

	SdlTypeInfo     m_targetTypeInfo;
	std::string     m_name;
	ExecuteFuncType m_func;
};

}// end namespace ph