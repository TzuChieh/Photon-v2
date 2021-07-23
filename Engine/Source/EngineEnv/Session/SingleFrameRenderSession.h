#pragma once

#include "EngineEnv/Session/RenderSession.h"
#include "Common/primitive_type.h"
#include "Math/TVector2.h"
#include "DataIO/SDL/sdl_interface.h"
#include "EngineEnv/sdl_accelerator_type.h"

#include <string>

namespace ph
{

class SingleFrameRenderSession : public RenderSession
{
public:
	inline SingleFrameRenderSession() = default;

	void applyToContext(CoreCookingContext& ctx) const override;
	std::vector<std::shared_ptr<CoreSdlResource>> gatherResources(const SceneDescription& scene) const override;

	void setTopLevelAccelerator(EAccelerator accelerator);

	const std::string& getVisualizerName() const;
	const std::string& getObserverName() const;
	const std::string& getSampleSourceName() const;
	EAccelerator getTopLevelAccelerator() const;

private:
	math::TVector2<uint32> m_frameSizePx;
	std::string            m_visualizerName;
	std::string            m_observerName;
	std::string            m_sampleSourceName;
	EAccelerator           m_topLevelAccelerator;

public:
	PH_DEFINE_SDL_CLASS(TOwnerSdlClass<SingleFrameRenderSession>)
	{
		ClassType clazz("single-frame-render-session");
		clazz.description("Information regarding the rendering process of a single frame.");
		clazz.baseOn<RenderSession>();

		TSdlVector2<OwnerType, uint32> frameSizePx("frame-size", &OwnerType::m_frameSizePx);
		frameSizePx.description("Width and height of the frame in pixels.");
		frameSizePx.defaultTo({960, 540});
		clazz.addField(frameSizePx);

		TSdlString<OwnerType> visualizerName("visualizer", &OwnerType::m_visualizerName);
		visualizerName.description("Name of the visualizer resource to use.");
		visualizerName.optional();
		clazz.addField(visualizerName);

		TSdlString<OwnerType> observerName("observer", &OwnerType::m_observerName);
		observerName.description("Name of the observer resource to use.");
		observerName.optional();
		clazz.addField(observerName);

		TSdlString<OwnerType> sampleSourceName("sample-source", &OwnerType::m_sampleSourceName);
		sampleSourceName.description("Name of the sample source resource to use.");
		sampleSourceName.optional();
		clazz.addField(sampleSourceName);

		TSdlEnumField<OwnerType, EAccelerator> topLevelAccelerator("top-level-accelerator", &OwnerType::m_topLevelAccelerator);
		topLevelAccelerator.description("Acceleration structure used on the top level geometries.");
		topLevelAccelerator.defaultTo(EAccelerator::BVH);
		topLevelAccelerator.optional();
		clazz.addField(topLevelAccelerator);

		return clazz;
	}
};

// In-header Implementations:

inline const std::string& SingleFrameRenderSession::getVisualizerName() const
{
	return m_visualizerName;
}

inline const std::string& SingleFrameRenderSession::getObserverName() const
{
	return m_observerName;
}

inline const std::string& SingleFrameRenderSession::getSampleSourceName() const
{
	return m_sampleSourceName;
}

inline void SingleFrameRenderSession::setTopLevelAccelerator(const EAccelerator accelerator)
{
	m_topLevelAccelerator = accelerator;
}

inline EAccelerator SingleFrameRenderSession::getTopLevelAccelerator() const
{
	return m_topLevelAccelerator;
}

}// end namespace ph
