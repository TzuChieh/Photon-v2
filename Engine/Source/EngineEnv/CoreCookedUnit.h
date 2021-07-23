#pragma once

#include "Utility/IMoveOnly.h"

#include <memory>
#include <vector>

namespace ph { class Renderer; };
namespace ph { class Receiver; };
namespace ph { class SampleGenerator; };
namespace ph::math { class Transform; };

namespace ph
{

class CoreCookedUnit final : public IMoveOnly
{
public:
	CoreCookedUnit();

	/*bool gatherFromRaw(const SceneDescription& scene);
	bool gatherFromCooked(const VisualWorld& world);*/

	void addRenderer(std::unique_ptr<Renderer> renderer);
	void addReceiver(std::unique_ptr<Receiver> receiver);
	void addSampleGenerator(std::unique_ptr<SampleGenerator> sampleGenerator);

	void addTransform(std::unique_ptr<math::Transform> transform);

	Renderer* getRenderer() const;
	Receiver* getReceiver() const;
	SampleGenerator* getSampleGenerator() const;

	/*Intersector* getIntersector() const;
	EmitterSampler* getEmitterSampler() const;
	Scene* getScene() const;*/

private:
	std::unique_ptr<Renderer>        m_renderer;
	std::unique_ptr<Receiver>        m_receiver;
	std::unique_ptr<SampleGenerator> m_sampleGenerator;

	std::vector<std::unique_ptr<math::Transform>> m_transforms;

	// Cooked
	/*std::shared_ptr<Intersector>     m_intersector;
	std::shared_ptr<EmitterSampler>  m_emitterSampler;
	std::shared_ptr<Scene>           m_scene;*/
};

// In-header Implementations:

inline Renderer* CoreCookedUnit::getRenderer() const
{
	return m_renderer.get();
}

inline Receiver* CoreCookedUnit::getReceiver() const
{ 
	return m_receiver.get();
}

inline SampleGenerator* CoreCookedUnit::getSampleGenerator() const
{
	return m_sampleGenerator.get();
}

inline void CoreCookedUnit::addTransform(std::unique_ptr<math::Transform> transform)
{
	PH_ASSERT(transform);

	m_transforms.push_back(std::move(transform));
}

}// end namespace ph
