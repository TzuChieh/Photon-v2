#pragma once

#include "Common/primitive_type.h"
#include "Core/Renderer/RendererProxy.h"

#include <atomic>

namespace ph
{

class RenderWork;

class RenderWorker final
{
public:
	class Progress final
	{
	public:
		uint64 totalWork;
		uint64 workDone;

		inline Progress() :
			totalWork(0), workDone(0)
		{

		}
	};

	inline RenderWorker() = default;
	RenderWorker(const RendererProxy& renderer, uint32 id);
	RenderWorker(const RenderWorker& other);

	void run();

	inline Progress queryProgress() const
	{
		Progress progress;
		progress.totalWork = static_cast<uint64>(m_totalWork);
		progress.workDone  = static_cast<uint64>(m_workDone);
		return progress;
	}

	RenderWorker& operator = (const RenderWorker& rhs);

private:
	RendererProxy        m_renderer;
	std::atomic_uint64_t m_totalWork;
	std::atomic_uint64_t m_workDone;
	uint32               m_id;

	void doWork(const RenderWork& work);
};

}// end namespace ph