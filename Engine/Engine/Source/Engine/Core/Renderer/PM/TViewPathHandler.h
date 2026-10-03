#pragma once

#include "Engine/Core/SurfaceHit.h"
#include "Engine/Core/Ray.h"
#include "Engine/Math/TVector2.h"
#include "Engine/Math/Color/Spectrum.h"
#include "Engine/Core/Renderer/PM/ViewPathTracingPolicy.h"
#include "Engine/Utility/utility.h"
#include "Engine/Utility/traits.h"

#include <cstddef>

namespace ph
{

class SampleFlow;

template<typename T>
concept CViewPathHandler = requires
{
	typename T::ViewPathHandlerTag;
};

/*! @brief Process and control a view path tracing process.
Derived classes need to implement all methods with "impl_" prefixed names and with the exact signature.
*/
template<typename Derived>
class TViewPathHandler
{
public:
	using ViewPathHandlerTag = void;

	/*! @brief Called after a receiver sample is generated.
	@return Whether this receiver sample should be used.
	*/
	bool onReceiverSampleBegin(
		const math::Vector2D& rasterCoord,
		const math::Vector2S& sampleIndex,
		const math::Spectrum& pathThroughput);

	/*! @brief Called after tracing a segment, before any surface-hit callback.
	Also called when no usable surface is reached.
	@param pathLength Current path length. The receiver's first segment has length 1.
	@param ray The traced segment, bounded by the nearest physical hit or the original ray limit.
	@param pathThroughput Throughput at the start of the segment.
	@param sampleFlow Sample stream for the traced path.
	*/
	void onPathTraced(
		std::size_t           pathLength,
		const Ray&            ray,
		const math::Spectrum& pathThroughput,
		SampleFlow&           sampleFlow);

	/*! @brief Called after the view path hits a surface, with corresponding hit information given.
	@return A policy for controlling how to trace the next path.
	*/
	auto onPathHitSurface(
		std::size_t           pathLength,
		const SurfaceHit&     surfaceHit,
		const math::Spectrum& pathThroughput) -> ViewPathTracingPolicy;

	/*! @brief Called after a receiver sample has ended.
	*/
	void onReceiverSampleEnd();

	/*! @brief Called after a batch of receiver samples has been consumed.
	Sample batch is as defined by the sample generator used.
	*/
	void onSampleBatchFinished();

// Hide special members as this class is not intended to be used polymorphically.
// It is derived class's choice to expose them (by defining them in public) or not.
protected:
	PH_DEFINE_INLINE_RULE_OF_5_MEMBERS(TViewPathHandler);
};

// In-header Implementations:

template<typename Derived>
bool TViewPathHandler<Derived>::onReceiverSampleBegin(
	const math::Vector2D& rasterCoord,
	const math::Vector2S& sampleIndex,
	const math::Spectrum& pathThroughput)
{
	static_assert(requires (
		Derived        derived,
		math::Vector2D rasterCoord, 
		math::Vector2S sampleIndex, 
		math::Spectrum pathThroughput)
		{
			{
				derived.impl_onReceiverSampleBegin(
					rasterCoord,
					sampleIndex, 
					pathThroughput)
			}
			-> CSame<bool>;
		},
		"A view path handler type must implement a method callable as "
		"`impl_onReceiverSampleBegin(math::Vector2D{}, math::Vector2S{}, math::Spectrum{}) -> bool`.");

	return static_cast<Derived&>(*this).impl_onReceiverSampleBegin(
		rasterCoord,
		sampleIndex,
		pathThroughput);
}

template<typename Derived>
void TViewPathHandler<Derived>::onPathTraced(
	const std::size_t     pathLength,
	const Ray&            ray,
	const math::Spectrum& pathThroughput,
	SampleFlow&           sampleFlow)
{
	static_assert(requires (
		Derived               derived,
		std::size_t           pathLength,
		const Ray&            ray,
		const math::Spectrum& pathThroughput,
		SampleFlow&           sampleFlow)
		{
			{ derived.impl_onPathTraced(pathLength, ray, pathThroughput, sampleFlow) } -> CSame<void>;
		},
		"A view path handler type must implement a method callable as "
		"`impl_onPathTraced(std::size_t pathLength, const Ray& ray, "
		"const math::Spectrum& pathThroughput, SampleFlow& sampleFlow) -> void`.");

	static_cast<Derived&>(*this).impl_onPathTraced(pathLength, ray, pathThroughput, sampleFlow);
}

template<typename Derived>
auto TViewPathHandler<Derived>::onPathHitSurface(
	const std::size_t       pathLength,
	const SurfaceHit&       surfaceHit,
	const math::Spectrum&   pathThroughput) -> ViewPathTracingPolicy
{
	static_assert(requires (
		Derived          derived,
		std::size_t      pathLength,
		SurfaceHit       surfaceHit,
		math::Spectrum   pathThroughput)
		{
			{
				derived.impl_onPathHitSurface(
					pathLength,
					surfaceHit,
					pathThroughput)
			}
			-> CSame<ViewPathTracingPolicy>;
		},
		"A view path handler type must implement a method callable as "
		"`impl_onPathHitSurface(std::size_t{}, SurfaceHit{}, math::Spectrum{}) -> ViewPathTracingPolicy`.");

	return static_cast<Derived&>(*this).impl_onPathHitSurface(
		pathLength,
		surfaceHit, 
		pathThroughput);
}

template<typename Derived>
void TViewPathHandler<Derived>::onReceiverSampleEnd()
{
	static_assert(requires (Derived derived)
		{
			{ derived.impl_onReceiverSampleEnd() } -> CSame<void>;
		},
		"A view path handler type must implement a method callable as "
		"`impl_onReceiverSampleEnd() -> void`.");

	static_cast<Derived&>(*this).impl_onReceiverSampleEnd();
}

template<typename Derived>
void TViewPathHandler<Derived>::onSampleBatchFinished()
{
	static_assert(requires (Derived derived)
		{
			{ derived.impl_onSampleBatchFinished() } -> CSame<void>;
		},
		"A view path handler type must implement a method callable as "
		"`impl_onSampleBatchFinished() -> void`.");

	static_cast<Derived&>(*this).impl_onSampleBatchFinished();
}

}// end namespace ph
