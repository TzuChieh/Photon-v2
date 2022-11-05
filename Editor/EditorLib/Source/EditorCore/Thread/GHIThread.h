#pragma once

#include "EditorCore/Thread/TUnbufferedFrameWorkerThread.h"

#include <memory>

namespace ph::editor
{

class GHI;

class GHIThread final : public TUnbufferedFrameWorkerThread<void(GHI&)>
{
public:
	using Base = TUnbufferedFrameWorkerThread<void(GHI&)>;

	GHIThread();
	~GHIThread() override;

	void onAsyncWorkerStart() override;
	void onAsyncWorkerStop() override;
	void onAsyncProcessWork(const Work& work) override;
	void onBeginFrame() override;
	void onEndFrame() override;

	void addSetGHIWork(GHI* inGHI);

private:
	void setGHI(GHI* inGHI);

	GHI*                 m_GHI;
	std::unique_ptr<GHI> m_nullGHI;
};

}// end namespace ph::editor
