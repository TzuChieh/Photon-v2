#pragma once

#include "EngineEnv/Visualizer/FrameVisualizer.h"
#include "Common/primitive_type.h"
#include "Math/TVector2.h"
#include "Math/Geometry/TAABB2D.h"
#include "DataIO/SDL/sdl_interface.h"
#include "EngineEnv/Visualizer/sdl_scheduler_type.h"
#include "EngineEnv/Visualizer/sdl_ray_energy_estimator_type.h"
#include "EngineEnv/Visualizer/sdl_sample_filter_type.h"
#include "Core/Filmic/SampleFilter.h"

#include <memory>

namespace ph { class IRayEnergyEstimator; }

namespace ph
{

class PathTracingVisualizer : public FrameVisualizer
{
public:
	inline PathTracingVisualizer() = default;

	void cook(const CoreCookingContext& ctx, CoreCookedUnit& cooked) override;

	EScheduler getScheduler() const;
	ERayEnergyEstimator getEstimator() const;
	ESampleFilter getSampleFilter() const;

protected:
	SampleFilter makeSampleFilter() const;
	std::unique_ptr<IRayEnergyEstimator> makeEstimator() const;

private:
	EScheduler          m_scheduler;
	ERayEnergyEstimator m_estimator;
	ESampleFilter       m_sampleFilter;

public:
	PH_DEFINE_SDL_CLASS(TOwnerSdlClass<PathTracingVisualizer>)
	{
		ClassType clazz("path-tracing");
		clazz.docName("Path Tracing Visualizer");
		clazz.description("Render frames with common path tracing methods.");
		clazz.baseOn<FrameVisualizer>();
		
		TSdlEnumField<OwnerType, EScheduler> scheduler("scheduler", &OwnerType::m_scheduler);
		scheduler.description("Scheduler for rendering, affect the order of rendered regions.");
		scheduler.defaultTo(EScheduler::SPIRAL_GRID);
		scheduler.optional();
		clazz.addField(scheduler);

		TSdlEnumField<OwnerType, ERayEnergyEstimator> estimator("estimator", &OwnerType::m_estimator);
		estimator.description("The energy estimating component used by the renderer.");
		estimator.defaultTo(ERayEnergyEstimator::BNEEPT);
		estimator.optional();
		clazz.addField(estimator);

		TSdlEnumField<OwnerType, ESampleFilter> sampleFilter("sample-filter", &OwnerType::m_sampleFilter);
		sampleFilter.description("Sample filter for the film sampling process.");
		sampleFilter.defaultTo(ESampleFilter::BLACKMAN_HARRIS);
		sampleFilter.optional();
		clazz.addField(sampleFilter);

		return clazz;
	}
};

// In-header Implementations:

inline EScheduler PathTracingVisualizer::getScheduler() const
{
	return m_scheduler;
}

inline ERayEnergyEstimator PathTracingVisualizer::getEstimator() const
{
	return m_estimator;
}

inline ESampleFilter PathTracingVisualizer::getSampleFilter() const
{
	return m_sampleFilter;
}

}// end namespace ph
