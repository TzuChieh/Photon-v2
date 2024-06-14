#pragma once

#include "Core/Renderer/RenderWork.h"
#include "Core/Renderer/PM/TViewPathHandler.h"
#include "Math/TVector2.h"
#include "Math/Geometry/TAABB2D.h"
#include "Math/Color/Spectrum.h"

#include <Common/primitive_type.h>

#include <cstddef>
#include <type_traits>
#include <vector>
#include <utility>

namespace ph
{

class Scene;
class Receiver;
class SampleGenerator;
class Ray;
class SampleFlow;

template<CViewPathHandler Handler>
class TViewPathTracingWork : public RenderWork
{
	static_assert(std::is_base_of_v<TViewPathHandler<Handler>, Handler>);

public:
	/*!
	@param handler Callback for handling path events.
	@param scene The scene to trace.
	@param receiver The receiver to start tracing from.
	@param sampleGenerator The sample generator to use.
	@param rasterSampleWindowPx The region to place samples.
	@param sampleRes The resolution of samples. This determines the amount and density of
	sample placement. For example, a sample resolution of `(a, b)` will place `a * b` samples
	within the sample window.
	*/
	TViewPathTracingWork(
		Handler*                      handler,
		const Scene*                  scene,
		const Receiver*               receiver,
		SampleGenerator*              sampleGenerator,
		const math::TAABB2D<float64>& rasterSampleWindowPx,
		const math::TVector2<int64>&  sampleRes);

	const math::Vector2S& getSampleRes() const;

private:
	void doWork() override;

	Handler*               m_handler;

	const Scene*           m_scene;
	const Receiver*        m_receiver;
	SampleGenerator*       m_sampleGenerator;
	math::TAABB2D<float64> m_rasterSampleWindowPx;
	math::Vector2S         m_sampleRes;

	void traceViewPath(
		SurfaceHit            prevHit,
		Ray                   tracingRay, 
		math::Spectrum        pathThroughput,
		std::size_t           pathLength,
		SampleFlow&           sampleFlow);

	void traceElementallyBranchedPath(
		const ViewPathTracingPolicy& policy,
		const SurfaceHit&            X,
		const math::Vector3R&        V,
		const math::Spectrum&        pathThroughput,
		std::size_t                  pathLength,
		SampleFlow&                  sampleFlow);
};

}// end namespace ph

#include "Core/Renderer/PM/TViewPathTracingWork.ipp"
