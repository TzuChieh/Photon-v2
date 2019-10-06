#pragma once

#include "Common/primitive_type.h"
#include "Math/Geometry/TAABB2D.h"
#include "Core/Renderer/RenderWork.h"

namespace ph
{

class Renderer;
class RenderWorker;

class RendererProxy final
{
public:
	using Region = math::TAABB2D<int64>;

public:
	RendererProxy();
	RendererProxy(Renderer* renderer);

	/*bool supplyWork(RenderWorker& worker);
	void submitWork(RenderWorker& worker);*/

private:
	Renderer* m_renderer;
};

}// end namespace ph