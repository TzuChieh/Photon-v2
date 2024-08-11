#pragma once

#include "EditorCore/Event/Event.h"

#include <Math/TVector2.h>
#include <Common/primitive_type.h>
#include <Common/utility.h>

namespace ph::editor
{

class DisplayFramebufferResizedEvent final : public Event
{
public:
	explicit DisplayFramebufferResizedEvent(math::TVector2<uint16> newSizePx);

	template<typename T>
	DisplayFramebufferResizedEvent(T widthPx, T heightPx);

	math::Vector2S getNewSizePx() const;

private:
	math::TVector2<uint16> m_newSizePx;
};

inline DisplayFramebufferResizedEvent::DisplayFramebufferResizedEvent(math::TVector2<uint16> newSizePx)
	: Event()
	, m_newSizePx(newSizePx)
{}

template<typename T>
inline DisplayFramebufferResizedEvent::DisplayFramebufferResizedEvent(T widthPx, T heightPx)
	: DisplayFramebufferResizedEvent(
		math::TVector2<uint16>(lossless_cast<uint16>(widthPx), lossless_cast<uint16>(heightPx)))
{}

inline math::Vector2S DisplayFramebufferResizedEvent::getNewSizePx() const
{
	return math::Vector2S(m_newSizePx);
}

}// end namespace ph::editor
