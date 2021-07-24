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
	math::TAABB2D<int64> getCropWindowPx() const;

private:
	math::TVector2<uint32> m_frameSizePx;

	int64 m_cropWindowXPx;
	int64 m_cropWindowYPx;
	int64 m_cropWindowWPx;
	int64 m_cropWindowHPx;

public:
	PH_DEFINE_SDL_CLASS(TOwnerSdlClass<FrameVisualizer>)
	{
		ClassType clazz("frame");
		clazz.description("A visualizer that produces frames, a typical example is an image.");
		clazz.baseOn<Visualizer>();
		
		TSdlVector2<OwnerType, uint32> frameSizePx("frame-size", &OwnerType::m_frameSizePx);
		frameSizePx.description("Width and height of the frame in pixels.");
		frameSizePx.defaultTo();

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

inline const math::TVector2<uint32>& FrameVisualizer::getFrameSizePx() const
{
	return m_frameSizePx;
}

inline math::TAABB2D<int64> FrameVisualizer::getCropWindowPx() const
{
	return math::TAABB2D<int64>(
		{m_cropWindowXPx, m_cropWindowYPx}, 
		{m_cropWindowXPx + m_cropWindowWPx, m_cropWindowYPx + m_cropWindowHPx});
}

}// end namespace ph
