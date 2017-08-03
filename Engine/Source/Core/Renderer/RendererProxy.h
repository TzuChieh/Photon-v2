#pragma once

#include "Common/primitive_type.h"
#include "Core/Bound/TAABB2D.h"
#include "Core/Renderer/RenderWork.h"

namespace ph
{

class Renderer;

class RendererProxy final
{
public:
	typedef TAABB2D<int64> Region;

public:
	RendererProxy();
	RendererProxy(Renderer* renderer);

	bool getNewWork(RenderWork* out_work);
	void submitWork(const RenderWork& work, bool isUpdating);

private:
	Renderer* m_renderer;
};

}// end namespace ph