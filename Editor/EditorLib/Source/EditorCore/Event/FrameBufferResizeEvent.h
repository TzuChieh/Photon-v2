#pragma once

#include "EditorCore/Event/Event.h"

#include <Math/TVector2.h>
#include <Common/primitive_type.h>
#include <Utility/utility.h>

namespace ph::editor
{

class FrameBufferResizeEvent final : public Event
{
public:
	explicit FrameBufferResizeEvent(math::TVector2<uint16> newSizePx);

	template<typename T>
	FrameBufferResizeEvent(T widthPx, T heightPx);

	math::Vector2S getNewSizePx() const;

private:
	math::TVector2<uint16> m_newSizePx;
};

inline FrameBufferResizeEvent::FrameBufferResizeEvent(math::TVector2<uint16> newSizePx)
	: Event()
	, m_newSizePx(newSizePx)
{}

template<typename T>
inline FrameBufferResizeEvent::FrameBufferResizeEvent(T widthPx, T heightPx)
	: FrameBufferResizeEvent(
		math::TVector2<uint16>(safe_number_cast<uint16>(widthPx), safe_number_cast<uint16>(heightPx)))
{}

inline math::Vector2S FrameBufferResizeEvent::getNewSizePx() const
{
	return math::Vector2S(m_newSizePx);
}

}// end namespace ph::editor
