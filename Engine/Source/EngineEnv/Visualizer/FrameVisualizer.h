#pragma once

#include "EngineEnv/Visualizer/Visualizer.h"
#include "Common/primitive_type.h"
#include "Math/TVector2.h"
#include "Math/Geometry/TAABB2D.h"
#include "DataIO/SDL/sdl_interface.h"

namespace ph
{

class FrameVisualizer : public Visualizer
{
public:
	inline FrameVisualizer() = default;

	virtual void cook(const CoreCookingContext& ctx, CoreCookedUnit& cooked) = 0;

	const math::TVector2<uint32>& getFrameSizePx() const;
	const math::TAABB2D<int64>& getCropWindowPx() const;

private:
	math::TAABB2D<int64> m_cropWindowPx;

public:
	PH_DEFINE_SDL_CLASS(TOwnerSdlClass<FrameVisualizer>)
	{
		ClassType clazz("frame");
		clazz.description("A visualizer that produces frames, a typical example is an image.");
		clazz.baseOn<Visualizer>();
		
		TSdlVector2<OwnerType, uint32> frameSizePx("frame-size", &OwnerType::m_frameSizePx);
		frameSizePx.description("Width and height of the frame in pixels.");
		frameSizePx.defaultTo();

		return clazz;
	}
};

// In-header Implementations:

inline const math::TVector2<uint32>& FrameVisualizer::getFrameSizePx() const
{
	return m_frameSizePx;
}

inline const math::TAABB2D<int64>& FrameVisualizer::getCropWindowPx() const
{
	return m_cropWindowPx;
}

}// end namespace ph
