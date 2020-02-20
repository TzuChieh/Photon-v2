#pragma once

#include "Core/Renderer/Renderer.h"
#include "Core/Filmic/SampleFilter.h"
#include "Core/Estimator/FullRayEnergyEstimator.h"

#include <memory>

namespace ph
{

class SamplingRenderer : public Renderer, public TCommandInterface<SamplingRenderer>
{
protected:
	SampleFilter                         m_filter;
	std::unique_ptr<IRayEnergyEstimator> m_estimator;

// command interface
public:
	explicit SamplingRenderer(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
};

}// end namespace ph

/*
	<SDL_interface>

	<category>  renderer          </category>
	<type_name> sampling          </type_name>
	<extend>    renderer.renderer </extend>

	<name> Sampling Renderer </name>

	<command type="creator" intent="blueprint">
		<input name="filter-name" type="string">
			<description>
				The type of filter used by the film. "box": box filter, fairly sharp but can have
				obvious aliasing around edges; "gaussian": Gaussian filter, gives smooth results;
				"mitchell-netravali" or "mn": Mitchell-Netravali filter, smooth but remains sharp
				around edges; "blackman-harris" or "bh": Blackman-Harris filter, a good compromise
				between smoothness and sharpness.
			</description>
		</input>
		<input name="estimator" type="string">
			<description>
				The energy estimating component used by the renderer. "bvpt": backward path 
				tracing; "bneept": backward path tracing with next event estimation.
			</description>
		</input>
	</command>

	</SDL_interface>
*/
