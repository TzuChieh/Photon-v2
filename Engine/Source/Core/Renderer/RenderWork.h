#pragma once

#include "Core/Filmic/filmic_fwd.h"
#include "Common/assertion.h"
#include "Core/Renderer/RenderProgress.h"

namespace ph
{

class RenderWorker;

class RenderWork
{
	friend class RenderWorker;

public:
	RenderWork();
	RenderWork(const RenderWork& other);
	virtual ~RenderWork();

	virtual void doWork() = 0;

protected:
	void setTotalWork(uint32 totalWork);
	void setWorkDone(uint32 workDone);
	void incrementWorkDone();

	RenderWork& operator = (const RenderWork& rhs);
	RenderWork& operator = (RenderWork&& rhs);

private:
	RenderWorker* m_worker;

	void setWorker(RenderWorker* worker);
};

// In-header Implementations:

inline RenderWork::RenderWork() : 
	m_worker(nullptr)
{}

inline RenderWork::RenderWork(const RenderWork& other) = default;

inline RenderWork::~RenderWork() = default;

inline void RenderWork::setWorker(RenderWorker* const worker)
{
	PH_ASSERT(worker);

	m_worker = worker;
}

inline RenderWork& RenderWork::operator = (const RenderWork& rhs)
{
	m_worker = rhs.m_worker;

	return *this;
}

inline RenderWork& RenderWork::operator = (RenderWork&& rhs)
{
	m_worker = std::move(rhs.m_worker);

	return *this;
}

}// end namespace ph