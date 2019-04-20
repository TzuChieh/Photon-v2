#pragma once

#include "Core/Renderer/Renderer.h"
#include "Core/Filmic/HdrRgbFilm.h"
#include "Core/Renderer/Sampling/CameraSamplingWork.h"
#include "Core/Renderer/Sampling/TCameraMeasurementEstimator.h"
#include "Core/Renderer/Region/WorkScheduler.h"
#include "Core/Renderer/Sampling/MetaRecordingProcessor.h"
#include "Core/Quantity/SpectralStrength.h"
#include "Math/TVector2.h"

#include <vector>
#include <memory>
#include <atomic>
#include <functional>
#include <string>

namespace ph
{

class Scene;
class Camera;
class SampleGenerator;

class AttributeRenderer : public Renderer, public TCommandInterface<AttributeRenderer>
{
public:
	void doUpdate(const SdlResourcePack& data) override;
	void doRender() override;
	void retrieveFrame(std::size_t layerIndex, HdrRgbFrame& out_frame) override;

	ERegionStatus asyncPollUpdatedRegion(Region* out_region) override;
	RenderState asyncQueryRenderState() override;
	RenderProgress asyncQueryRenderProgress() override;
	void asyncPeekFrame(
		std::size_t   layerIndex,
		const Region& region,
		HdrRgbFrame&  out_frame) override;

	ObservableRenderData getObservableData() const override;

private:
	const Scene*     m_scene;
	const Camera*    m_camera;
	SampleGenerator* m_sampleGenerator;

	std::string      m_attributeName;
	HdrRgbFilm       m_attributeFilm;
	
	std::mutex       m_rendererMutex;

// command interface
public:
	explicit AttributeRenderer(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
};

}// end namespace ph

/*
	<SDL_interface>

	<category>  renderer          </category>
	<type_name> attribute         </type_name>
	<extend>    renderer.renderer </extend>

	<name> Attribute Renderer </name>
	<description>
		This renderer produces various type of attributes which can be useful
		for compositing. The attributes are also known as AOVs (arbitrary 
		output variables).
	</description>

	<command type="creator">
		<input name="attribute" type="string">
			<description>The attribute to render.</description>
		</input>
	</command>

	</SDL_interface>
*/