#pragma once

#include "FileIO/SDL/ISdlResource.h"

#include <functional>
#include <memory>

namespace ph
{

class InputPacket;

class SdlLoader final
{
private:
	typedef std::function
	<
		std::unique_ptr<ISdlResource>(const InputPacket& packet)
	> LoadFuncType;

public:
	inline SdlLoader() :
		m_func(nullptr)
	{

	}

	inline void setFunc(const LoadFuncType& func)
	{
		m_func = func;
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
	LoadFuncType m_func;
};

}// end namespace ph