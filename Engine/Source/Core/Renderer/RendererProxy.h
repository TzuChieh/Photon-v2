#pragma once

#include "Common/primitive_type.h"
#include "Core/Bound/TAABB2D.h"

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

	void asyncAddUpdatedRegion(const Region& region);

private:
	Renderer* m_renderer;
};

}// end namespace ph