#pragma once

#include "EditorCore/Event/Event.h"

#include <Math/TVector2.h>
#include <Common/primitive_type.h>
#include <Utility/utility.h>

namespace ph::editor
{

class DisplayFramebufferResizeEvent final : public Event
{
public:
	explicit DisplayFramebufferResizeEvent(math::TVector2<uint16> newSizePx);

	template<typename T>
	DisplayFramebufferResizeEvent(T widthPx, T heightPx);

	math::Vector2S getNewSizePx() const;

private:
	math::TVector2<uint16> m_newSizePx;
};

inline DisplayFramebufferResizeEvent::DisplayFramebufferResizeEvent(math::TVector2<uint16> newSizePx)
	: Event()
	, m_newSizePx(newSizePx)
{}

template<typename T>
inline DisplayFramebufferResizeEvent::DisplayFramebufferResizeEvent(T widthPx, T heightPx)
	: FramebufferResizeEvent(
		math::TVector2<uint16>(lossless_cast<uint16>(widthPx), lossless_cast<uint16>(heightPx)))
{}

inline math::Vector2S DisplayFramebufferResizeEvent::getNewSizePx() const
{
	return math::Vector2S(m_newSizePx);
}

}// end namespace ph::editor
