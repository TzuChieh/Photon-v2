#pragma once

#include "DataIO/SDL/ExitStatus.h"
#include "DataIO/SDL/SdlTypeInfo.h"

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
		m_func = [](const SdlExecutor& thiz, 
		            const std::shared_ptr<ISdlResource>& res,
		            const InputPacket& pac) -> ExitStatus
		{
			return ExitStatus::FAILURE(std::string()
				+ "warning: at SdlExecutor, "
				+ "executor <"
				+ thiz.m_name
				+ "> with target type <"
				+ thiz.m_targetTypeInfo.toString()
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

	// FIXME: potentially bad or even wrong spec., revision needed
	// Functions operate on target types other than the owner type must make sure the
	// desired target type is a base class of the owner type. This will ensure correct
	// is-a relationship
	template
	<
		typename OwnerType, 
		typename TargetType,
		typename = std::enable_if_t<std::is_base_of<ISdlResource, TargetType>::value>,
		typename = std::enable_if_t<std::is_base_of<TargetType, OwnerType>::value>
	>
	inline void setFunc(const ExecuteFuncType<TargetType>& func)
	{
		m_targetTypeInfo = TargetType::ciTypeInfo();

		m_func = [func](const SdlExecutor& thiz, 
		                const std::shared_ptr<ISdlResource>& res,
		                const InputPacket& pac) -> ExitStatus
		{
			// Ensuring that the input resource is convertible to owner's type to
			// prevent possible misuse.
			// Input target resource is allowed to be null since executor may not 
			// necessarily operate on resources, so we only check for situations
			// where input target resource is non-null.
			const auto& ownerRes = std::dynamic_pointer_cast<OwnerType>(res);
			if(res != nullptr && ownerRes == nullptr)
			{
				return ExitStatus::BAD_INPUT(std::string()
					+ "warning: at SdlExecutor, " 
					+ "executor <"
					+ thiz.m_name
					+ "> accepts only SDL resources of type <"
					+ thiz.m_targetTypeInfo.toString()
					+ "> (casting failed)");
			}

			return func(ownerRes, pac);
		};
	}

	inline ExitStatus execute(
		const std::shared_ptr<ISdlResource>& targetResource,
		const InputPacket& packet) const
	{
		return m_func(*this, targetResource, packet);
	}

	inline std::string getName() const
	{
		return m_name;
	}

	inline SdlTypeInfo getTargetTypeInfo() const
	{
		return m_targetTypeInfo;
	}

	inline bool isValid() const
	{
		return m_targetTypeInfo.isValid() && !m_name.empty() && m_func != nullptr;
	}

	inline std::string toString() const
	{
		return std::string()
			+ "SDL executor <"
			+ m_name
			+ "> for type <"
			+ m_targetTypeInfo.toString()
			+ ">";
	}

private:
	SdlTypeInfo m_targetTypeInfo;
	std::string m_name;

	std::function
	<
		ExitStatus(const SdlExecutor& thiz, 
		           const std::shared_ptr<ISdlResource>& targetResource,
		           const InputPacket& packet)
	> m_func;
};

}// end namespace ph
