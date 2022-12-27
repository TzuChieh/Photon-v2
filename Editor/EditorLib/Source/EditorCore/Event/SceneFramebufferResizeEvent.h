#pragma once

#include "EditorCore/Event/Event.h"

#include <Math/TVector2.h>
#include <Common/primitive_type.h>
#include <Utility/utility.h>

namespace ph::editor
{

class SceneFramebufferResizeEvent final : public Event
{
public:
	SceneFramebufferResizeEvent(math::TVector2<uint16> newSizePx, std::size_t sceneIndex);

	template<typename T>
	SceneFramebufferResizeEvent(T widthPx, T heightPx, std::size_t sceneIndex);

	math::Vector2S getNewSizePx() const;
	std::size_t getSceneIndex() const;

private:
	math::TVector2<uint16> m_newSizePx;
	uint8 m_sceneIndex;
};

inline SceneFramebufferResizeEvent::SceneFramebufferResizeEvent(
	const math::TVector2<uint16> newSizePx, 
	const std::size_t sceneIndex)

	: Event()

	, m_newSizePx(newSizePx)
	, m_sceneIndex(lossless_cast<uint8>(sceneIndex))
{}

template<typename T>
inline SceneFramebufferResizeEvent::SceneFramebufferResizeEvent(
	const T widthPx, 
	const T heightPx, 
	const std::size_t sceneIndex)

	: SceneFramebufferResizeEvent(
		math::TVector2<uint16>(lossless_cast<uint16>(widthPx), lossless_cast<uint16>(heightPx)),
		sceneIndex)
{}

inline math::Vector2S SceneFramebufferResizeEvent::getNewSizePx() const
{
	return math::Vector2S(m_newSizePx);
}

inline std::size_t SceneFramebufferResizeEvent::getSceneIndex() const
{
	return m_sceneIndex;
}

}// end namespace ph::editor
