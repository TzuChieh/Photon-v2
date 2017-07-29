#pragma once

#include "Common/primitive_type.h"
#include "Core/Renderer/RenderData.h"

#include <atomic>

namespace ph
{

class RenderWorker final
{
public:
	class Progress final
	{
	public:
		uint32 totalWork;
		uint32 workDone;
	};

	RenderData data;

	inline RenderWorker() = default;
	RenderWorker(const RenderData& data);
	RenderWorker(const RenderWorker& other);

	void run();

	inline Progress queryProgress() const
	{
		Progress progress;
		progress.totalWork = static_cast<uint32>(m_totalWork);
		progress.workDone  = static_cast<uint32>(m_workDone);
		return progress;
	}

	RenderWorker& operator = (const RenderWorker& rhs);

private:
	std::atomic_uint32_t m_totalWork;
	std::atomic_uint32_t m_workDone;
};

}// end namespace ph