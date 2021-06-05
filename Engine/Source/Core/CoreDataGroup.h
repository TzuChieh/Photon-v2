#pragma once

#include <memory>

namespace ph
{

class Renderer;
class Receiver;
class SampleGenerator;
class CookSettings;
class SceneDescription;
class VisualWorld;
class Intersector;
class EmitterSampler;
class Scene;

class CoreDataGroup final
{
public:
	CoreDataGroup();

	bool gatherFromRaw(const SceneDescription& scene);
	bool gatherFromCooked(const VisualWorld& world);

	Renderer* getRenderer() const;
	Receiver* getReceiver() const;
	SampleGenerator* getSampleGenerator() const;
	CookSettings* getCookSettings() const;

	Intersector* getIntersector() const;
	EmitterSampler* getEmitterSampler() const;
	Scene* getScene() const;

private:
	// Raw
	std::shared_ptr<Renderer>        m_renderer;
	std::shared_ptr<Receiver>        m_receiver;
	std::shared_ptr<SampleGenerator> m_sampleGenerator;
	std::shared_ptr<CookSettings>    m_cookSettings;

	// Cooked
	std::shared_ptr<Intersector>     m_intersector;
	std::shared_ptr<EmitterSampler>  m_emitterSampler;
	std::shared_ptr<Scene>           m_scene;
};

// In-header Implementations:


inline Renderer* CoreDataGroup::getRenderer() const
{
	return m_renderer.get();
};

inline Receiver* CoreDataGroup::getReceiver() const
{ 
	return m_receiver.get();
};

inline SampleGenerator* CoreDataGroup::getSampleGenerator() const
{
	return m_sampleGenerator.get();
};

inline CookSettings* CoreDataGroup::getCookSettings() const
{
	return m_cookSettings.get();
};

inline Intersector* CoreDataGroup::getIntersector() const
{
	return m_intersector.get();
}

inline EmitterSampler* CoreDataGroup::getEmitterSampler() const
{
	return m_emitterSampler.get();
}

inline Scene* CoreDataGroup::getScene() const
{
	return m_scene.get();
}

}// end namespace ph
