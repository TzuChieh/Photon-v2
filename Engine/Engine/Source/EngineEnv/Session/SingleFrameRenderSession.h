#pragma once

#include "EngineEnv/Session/RenderSession.h"
#include "Math/TVector2.h"
#include "SDL/sdl_interface.h"
#include "EngineEnv/sdl_accelerator_type.h"

#include <Common/primitive_type.h>

#include <string>

namespace ph
{

class SingleFrameRenderSession : public RenderSession
{
public:
	void applyToContext(CoreCookingContext& ctx) const override;
	std::vector<std::shared_ptr<CoreSdlResource>> gatherResources(const SceneDescription& scene) const override;

	void setTopLevelAcceleratorType(EAccelerator accelerator);

	const std::string& getVisualizerName() const;
	const std::string& getObserverName() const;
	const std::string& getSampleSourceName() const;
	EAccelerator getTopLevelAcceleratorType() const;

private:
	math::TVector2<uint32> m_frameSizePx;
	std::string            m_visualizerName;
	std::string            m_observerName;
	std::string            m_sampleSourceName;
	EAccelerator           m_topLevelAcceleratorType;

public:
	PH_DEFINE_SDL_CLASS(TSdlOwnerClass<SingleFrameRenderSession>)
	{
		ClassType clazz("single-frame-render-session");
		clazz.docName("Single Frame Render Session");
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

		TSdlEnumField<OwnerType, EAccelerator> topLevelAcceleratorType("top-level-accelerator", &OwnerType::m_topLevelAcceleratorType);
		topLevelAcceleratorType.description("Acceleration structure used on the top level geometries.");
		topLevelAcceleratorType.defaultTo(EAccelerator::BVH);
		topLevelAcceleratorType.optional();
		clazz.addField(topLevelAcceleratorType);

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

inline void SingleFrameRenderSession::setTopLevelAcceleratorType(const EAccelerator accelerator)
{
	m_topLevelAcceleratorType = accelerator;
}

inline EAccelerator SingleFrameRenderSession::getTopLevelAcceleratorType() const
{
	return m_topLevelAcceleratorType;
}

}// end namespace ph
