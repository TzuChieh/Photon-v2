#pragma once

#include "EngineEnv/Visualizer/FrameVisualizer.h"
#include "Common/primitive_type.h"
#include "Math/TVector2.h"
#include "Math/Geometry/TAABB2D.h"
#include "DataIO/SDL/sdl_interface.h"

namespace ph
{

class PathTracingVisualizer : public FrameVisualizer
{
public:
	inline PathTracingVisualizer() = default;

	virtual void cook(const CoreCookingContext& ctx, CoreCookedUnit& cooked) = 0;

private:
	math::TAABB2D<int64> m_cropWindowPx;

public:
	PH_DEFINE_SDL_CLASS(TOwnerSdlClass<PathTracingVisualizer>)
	{
		ClassType clazz("path-tracing");
		clazz.description("Render frames with common path tracing methods.");
		clazz.baseOn<FrameVisualizer>();
		
		TSdlVector2<OwnerType, uint32> frameSizePx("frame-size", &OwnerType::m_frameSizePx);
		frameSizePx.description("Width and height of the frame in pixels.");
		frameSizePx.defaultTo();

		return clazz;
	}
};

}// end namespace ph
