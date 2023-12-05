#pragma once

#include <Common/assertion.h>

namespace ph
{

class Scene;
class Receiver;

class Integrand final
{
public:
	Integrand();
	Integrand(const Scene* scene, const Receiver* receiver);

	const Scene& getScene() const;
	const Receiver& getReceiver() const;

private:
	const Scene*    m_scene;
	const Receiver* m_receiver;
};

// In-header Implementations:

inline Integrand::Integrand() : 
	m_scene   (nullptr),
	m_receiver(nullptr)
{}

inline Integrand::Integrand(const Scene* const scene, const Receiver* const receiver) :
	m_scene   (scene),
	m_receiver(receiver)
{
	PH_ASSERT(scene);
	PH_ASSERT(receiver);
}

inline const Scene& Integrand::getScene() const
{
	PH_ASSERT(m_scene);

	return *m_scene;
}

inline const Receiver& Integrand::getReceiver() const
{
	PH_ASSERT(m_receiver);

	return *m_receiver;
}

}// end namespace ph
