#pragma once

#include "Common/primitive_type.h"
#include "Core/Bound/TAABB2D.h"
#include "Core/Renderer/RenderWork.h"

namespace ph
{

class Renderer;
class RenderWorker;

class RendererProxy final
{
public:
	typedef TAABB2D<int64> Region;

public:
	RendererProxy();
	RendererProxy(Renderer* renderer);

	bool supplyWork(RenderWorker& worker);
	void submitWork(RenderWorker& worker);

private:
	Renderer* m_renderer;
};

}// end namespace ph