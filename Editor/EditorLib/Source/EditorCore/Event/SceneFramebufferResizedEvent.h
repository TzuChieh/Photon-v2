#pragma once

#include "EditorCore/Event/Event.h"

#include <Math/TVector2.h>
#include <Common/primitive_type.h>
#include <Utility/utility.h>

namespace ph::editor
{

class SceneFramebufferResizedEvent final : public Event
{
public:
	SceneFramebufferResizedEvent(math::TVector2<uint16> newSizePx, std::size_t sceneIndex);

	template<typename T>
	SceneFramebufferResizedEvent(T widthPx, T heightPx, std::size_t sceneIndex);

	math::Vector2S getNewSizePx() const;
	std::size_t getSceneIndex() const;

private:
	math::TVector2<uint16> m_newSizePx;
	uint8 m_sceneIndex;
};

inline SceneFramebufferResizedEvent::SceneFramebufferResizedEvent(
	const math::TVector2<uint16> newSizePx, 
	const std::size_t sceneIndex)

	: Event()

	, m_newSizePx(newSizePx)
	, m_sceneIndex(lossless_cast<uint8>(sceneIndex))
{}

template<typename T>
inline SceneFramebufferResizedEvent::SceneFramebufferResizedEvent(
	const T widthPx, 
	const T heightPx, 
	const std::size_t sceneIndex)

	: SceneFramebufferResizedEvent(
		math::TVector2<uint16>(lossless_cast<uint16>(widthPx), lossless_cast<uint16>(heightPx)),
		sceneIndex)
{}

inline math::Vector2S SceneFramebufferResizedEvent::getNewSizePx() const
{
	return math::Vector2S(m_newSizePx);
}

inline std::size_t SceneFramebufferResizedEvent::getSceneIndex() const
{
	return m_sceneIndex;
}

}// end namespace ph::editor
