#pragma once

#include "Common/assertion.h"

namespace ph
{

class Scene;
class Camera;

class Integrand final
{
public:
	Integrand();
	Integrand(const Scene* scene, const Camera* camera);

	const Scene& getScene() const;
	const Camera& getCamera() const;

private:
	const Scene*  m_scene;
	const Camera* m_camera;
};

// In-header Implementations:

inline Integrand::Integrand() : 
	Integrand(nullptr, nullptr)
{}

inline Integrand::Integrand(const Scene* const scene, const Camera* const camera) : 
	m_scene(scene),
	m_camera(camera)
{}

inline const Scene& Integrand::getScene() const
{
	PH_ASSERT(m_scene);

	return *m_scene;
}

inline const Camera& Integrand::getCamera() const
{
	PH_ASSERT(m_camera);

	return *m_camera;
}

}// end namespace ph
