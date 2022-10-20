#pragma once

#include "App/Event/Event.h"

#include <Math/TVector2.h>

namespace ph::editor
{

class FrameBufferResizeEvent final : public Event
{
public:
	explicit FrameBufferResizeEvent(math::Vector2S newSizePx);

	math::Vector2S getNewSizePx() const;

private:
	math::Vector2S m_newSizePx;
};

inline FrameBufferResizeEvent::FrameBufferResizeEvent(math::Vector2S newSizePx)
	: Event()
	, m_newSizePx(newSizePx)
{}

inline math::Vector2S FrameBufferResizeEvent::getNewSizePx() const
{
	return m_newSizePx;
}

}// end namespace ph::editor
