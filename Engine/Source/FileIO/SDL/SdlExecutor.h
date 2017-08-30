#pragma once

#include "FileIO/SDL/CommandEntry.h"
#include "FileIO/SDL/ExitStatus.h"
#include "FileIO/SDL/SdlTypeInfo.h"

#include <string>
#include <functional>
#include <memory>
#include <type_traits>

namespace ph
{

class InputPacket;

class SdlExecutor final
{
public:
	template<typename TargetType>
	using ExecuteFuncType = std::function
		<
			ExitStatus(const std::shared_ptr<TargetType>& targetResource,
			           const InputPacket& packet)
		>;

	inline SdlExecutor() :
		m_targetTypeInfo(SdlTypeInfo::makeInvalid()),
		m_name(),
		m_func(nullptr)
	{
		m_func = [this](const std::shared_ptr<ISdlResource>& res,
		                const InputPacket& pac) -> ExitStatus
		{
			return ExitStatus::FAILURE(std::string()
				+ "warning: at SdlExecutor, "
				+ "executor <"
				+ m_name
				+ "> with target type <"
				+ m_targetTypeInfo.toString()
				+ "> is empty");
		};
	}

	inline void setName(const std::string& name)
	{
		m_name = name;
	}

	template<typename OwnerType>
	inline void setFunc(const ExecuteFuncType<OwnerType>& func)
	{
		setFunc<OwnerType, OwnerType>(func);
	}

	// Functions operate on target types other than the owner type must make sure the
	// desired target type is a base class of the owner type. This will ensure correct
	// is-a relationship. We force this criterion using SFINAE.

	template
	<
		typename OwnerType, 
		typename TargetType,
		typename = std::enable_if_t<std::is_base_of<TargetType, OwnerType>::value>
	>
	inline void setFunc(const ExecuteFuncType<TargetType>& func)
	{
		m_targetTypeInfo = TargetType::ciTypeInfo();

		m_func = [this, func](const std::shared_ptr<ISdlResource>& res,
		                      const InputPacket& pac) -> ExitStatus
		{
			// input target resource is allowed to be null since executor may not 
			// necessarily operate on resources, so we only check for situations
			// where input target resource is non-null
			const std::shared_ptr<OwnerType>& ownerRes = std::dynamic_pointer_cast<OwnerType>(res);
			if(res != nullptr && ownerRes == nullptr)
			{
				return ExitStatus::BAD_INPUT(std::string()
					+ "warning: at SdlExecutor, " 
					+ "executor <"
					+ m_name
					+ "> accepts only SDL resources of type <"
					+ m_targetTypeInfo.toString()
					+ "> (casting failed)");
			}

			return func(ownerRes, pac);
		}
	}

	template
	<
		typename OwnerType,
		typename TargetType,
		typename = std::enable_if_t<!std::is_base_of<TargetType, OwnerType>::value>
	>
	inline void setFunc(const ExecuteFuncType<TargetType>& func)
	{
		m_targetTypeInfo = TargetType::ciTypeInfo();

		m_func = [this](const std::shared_ptr<ISdlResource>& res,
		                const InputPacket& pac) -> ExitStatus
		{
			return ExitStatus::FAILURE(std::string()
				+ "warning: at SdlExecutor, "
				+ "executor <"
				+ m_name
				+ "> with target type <"
				+ m_targetTypeInfo.toString()
				+ "> does not comply with the correct is-a relationship");
		};
	}

	inline ExitStatus execute(
		const std::shared_ptr<ISdlResource>& targetResource,
		const InputPacket& packet) const
	{
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
	SdlTypeInfo                   m_targetTypeInfo;
	std::string                   m_name;
	ExecuteFuncType<ISdlResource> m_func;
};

}// end namespace ph