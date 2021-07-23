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

	void cook(const CoreCookingContext& ctx, CoreCookedUnit& cooked) override;

	math::TAABB2D<int64> getCropWindowPx() const;

private:
	int64 m_cropWindowXPx;
	int64 m_cropWindowYPx;
	int64 m_cropWindowWPx;
	int64 m_cropWindowHPx;

public:
	PH_DEFINE_SDL_CLASS(TOwnerSdlClass<PathTracingVisualizer>)
	{
		ClassType clazz("path-tracing");
		clazz.description("Render frames with common path tracing methods.");
		clazz.baseOn<FrameVisualizer>();
		
		TSdlInt64<OwnerType> cropWindowXPx("rect-x", &OwnerType::m_cropWindowXPx);
		cropWindowXPx.description("X coordinate of the lower-left corner of the film cropping window.");
		cropWindowXPx.defaultTo(0);
		cropWindowXPx.optional();
		clazz.addField(cropWindowXPx);

		TSdlInt64<OwnerType> cropWindowYPx("rect-y", &OwnerType::m_cropWindowYPx);
		cropWindowYPx.description("Y coordinate of the lower-left corner of the film cropping window.");
		cropWindowYPx.defaultTo(0);
		cropWindowYPx.optional();
		clazz.addField(cropWindowYPx);

		TSdlInt64<OwnerType> cropWindowWPx("rect-w", &OwnerType::m_cropWindowWPx);
		cropWindowWPx.description("Width of the film cropping window.");
		cropWindowWPx.defaultTo(0);
		cropWindowWPx.optional();
		clazz.addField(cropWindowWPx);

		TSdlInt64<OwnerType> cropWindowHPx("rect-h", &OwnerType::m_cropWindowHPx);
		cropWindowHPx.description("Height of the film cropping window.");
		cropWindowHPx.defaultTo(0);
		cropWindowHPx.optional();
		clazz.addField(cropWindowHPx);

		return clazz;
	}
};

// In-header Implementations:

inline math::TAABB2D<int64> PathTracingVisualizer::getCropWindowPx() const
{
	return math::TAABB2D<int64>(
		{m_cropWindowXPx, m_cropWindowYPx}, 
		{m_cropWindowXPx + m_cropWindowWPx, m_cropWindowYPx + m_cropWindowHPx});
}

}// end namespace ph
