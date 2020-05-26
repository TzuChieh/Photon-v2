#pragma once

#include "DataIO/SDL/ISdlResource.h"

#include <functional>
#include <memory>

namespace ph
{

class InputPacket;

class SdlLoader final
{
public:
	typedef std::function
	<
		std::unique_ptr<ISdlResource>(const InputPacket& packet)
	> LoadFuncType;

	inline SdlLoader() :
		m_func(nullptr)
	{}

	inline explicit SdlLoader(const LoadFuncType& func) :
		m_func(func)
	{}

	template<typename T>
	inline void setFunc(const LoadFuncType& func)
	{
		// TODO: potentially eliminate wrapper when func already matches the signature
		// (or, do we even need the wrapper? always implicit cast and check types here?)

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
	LoadFuncType m_func;
};

}// end namespace ph
