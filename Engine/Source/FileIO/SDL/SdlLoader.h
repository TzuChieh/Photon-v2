#pragma once

#include "FileIO/SDL/ISdlResource.h"

#include <functional>
#include <memory>

namespace ph
{

class InputPacket;

class SdlLoader final
{
public:
	inline SdlLoader() :
		m_func(nullptr)
	{

	}

	template<typename T>
	inline void setFunc(
		const std::function
		<
			std::unique_ptr<T>(const InputPacket& packet)
		>& func)
	{
		m_func = [func](const InputPacket& pac)
			-> std::unique_ptr<ISdlResource>
		{
			return func(pac);
		};
	}

	inline std::unique_ptr<ISdlResource> load(const InputPacket& packet) const
	{
		return m_func != nullptr ? m_func(packet) : nullptr;
	}

	inline bool isValid() const
	{
		return m_func != nullptr;
	}

private:
	typedef std::function
	<
		std::unique_ptr<ISdlResource>(const InputPacket& packet)
	> LoadFuncType;

	LoadFuncType m_func;
};

}// end namespace ph