#pragma once

#include "EditorCore/Event/Event.h"

#include <Math/TVector2.h>
#include <Common/primitive_type.h>
#include <Utility/utility.h>

namespace ph::editor
{

class FramebufferResizeEvent final : public Event
{
public:
	explicit FramebufferResizeEvent(math::TVector2<uint16> newSizePx);

	template<typename T>
	FramebufferResizeEvent(T widthPx, T heightPx);

	math::Vector2S getNewSizePx() const;

private:
	math::TVector2<uint16> m_newSizePx;
};

inline FramebufferResizeEvent::FramebufferResizeEvent(math::TVector2<uint16> newSizePx)
	: Event()
	, m_newSizePx(newSizePx)
{}

template<typename T>
inline FramebufferResizeEvent::FramebufferResizeEvent(T widthPx, T heightPx)
	: FramebufferResizeEvent(
		math::TVector2<uint16>(safe_number_cast<uint16>(widthPx), safe_number_cast<uint16>(heightPx)))
{}

inline math::Vector2S FramebufferResizeEvent::getNewSizePx() const
{
	return math::Vector2S(m_newSizePx);
}

}// end namespace ph::editor
