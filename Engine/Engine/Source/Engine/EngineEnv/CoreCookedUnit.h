#pragma once

#include "Engine/Utility/IMoveOnly.h"
#include "Engine/Core/Transform/Transform.h"
#include "Engine/EngineEnv/Visualizer/FilmSetting.h"

#include <Common/assertion.h>

#include <memory>
#include <vector>

namespace ph { class Renderer; };
namespace ph { class Receiver; };
namespace ph { class SampleGenerator; };

namespace ph
{

class CoreCookedUnit final : private IMoveOnly
{
public:
	CoreCookedUnit();
	~CoreCookedUnit();

	/*bool gatherFromRaw(const SceneDescription& scene);
	bool gatherFromCooked(const VisualWorld& world);*/

	/*! @brief Install a renderer and its ordered output film settings.
	*/
	void addRenderer(
		std::unique_ptr<Renderer> renderer,
		std::vector<FilmSetting> filmSettings);

	void addReceiver(std::unique_ptr<Receiver> receiver);
	void addSampleGenerator(std::unique_ptr<SampleGenerator> sampleGenerator);

	void addTransform(std::unique_ptr<Transform> transform);

	Renderer* getRenderer() const;

	/*! @brief Get settings corresponding one-to-one with Renderer frame layer indices.
	*/
	const std::vector<FilmSetting>& getFilmSettings() const;

	Receiver* getReceiver() const;
	SampleGenerator* getSampleGenerator() const;

	/*Intersector* getIntersector() const;
	EmitterSampler* getEmitterSampler() const;
	Scene* getScene() const;*/

private:
	std::unique_ptr<Renderer>        m_renderer;
	std::unique_ptr<Receiver>        m_receiver;
	std::unique_ptr<SampleGenerator> m_sampleGenerator;
	std::vector<FilmSetting>         m_filmSettings;

	std::vector<std::unique_ptr<Transform>> m_transforms;

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

inline const std::vector<FilmSetting>& CoreCookedUnit::getFilmSettings() const
{
	return m_filmSettings;
}

inline Receiver* CoreCookedUnit::getReceiver() const
{
	return m_receiver.get();
}

inline SampleGenerator* CoreCookedUnit::getSampleGenerator() const
{
	return m_sampleGenerator.get();
}

inline void CoreCookedUnit::addTransform(std::unique_ptr<Transform> transform)
{
	PH_ASSERT(transform);

	m_transforms.push_back(std::move(transform));
}

}// end namespace ph
