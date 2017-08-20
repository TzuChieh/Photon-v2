#pragma once

#include "Common/primitive_type.h"
#include "Core/Renderer/RendererProxy.h"
#include "Core/Renderer/Statistics.h"

#include <atomic>
#include <mutex>
#include <utility>

namespace ph
{

class RenderWork;

class RenderWorker final
{
public:
	inline RenderWorker() = default;
	RenderWorker(const RendererProxy& renderer, uint32 id);
	RenderWorker(const RenderWorker& other);

	void run();
	Statistics::Record getStatistics() const;

	RenderWorker& operator = (const RenderWorker& rhs);

private:
	RendererProxy m_renderer;
	uint32        m_id;
	Statistics    m_statistics;

	void doWork(RenderWork& work);
};

}// end namespace ph