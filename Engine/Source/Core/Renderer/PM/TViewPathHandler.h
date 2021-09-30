#pragma once

#include "Core/SurfaceHit.h"
#include "Math/TVector2.h"
#include "Math/Color/Spectrum.h"
#include "Core/Renderer/PM/ViewPathTracingPolicy.h"

#include <cstddef>

namespace ph
{

/*
	Controls a view path tracing process. Derived classes need to implement
	all methods with "impl_" prefixed names and with the exact signature.
*/
template<typename Derived>
class TViewPathHandler
{
	friend Derived;

public:
	// Called after a receiver sample is generated.
	// Returns whether this receiver sample should be used.
	bool onReceiverSampleStart(
		const math::Vector2D& rasterCoord,
		const math::Spectrum& pathThroughput);

	// Called after the view path hits a surface, corresponding hit information
	// is given.
	// Returns a policy for controlling how to trace the next path.
	auto onPathHitSurface(
		std::size_t           pathLength,
		const SurfaceHit&     surfaceHit,
		const math::Spectrum& pathThroughput) -> ViewPathTracingPolicy;

	// Called after a receiver sample is ended.
	void onReceiverSampleEnd();

	// Called after a batch of receiver samples has been consumed.
	void onSampleBatchFinished();

private:
	TViewPathHandler() = default;
	~TViewPathHandler() = default;
};

// In-header Implementations:

template<typename Derived>
bool TViewPathHandler<Derived>::onReceiverSampleStart(
	const math::Vector2D& rasterCoord,
	const math::Spectrum& pathThroughput)
{
	return static_cast<Derived&>(*this).impl_onReceiverSampleStart(
		rasterCoord,
		pathThroughput);
}

template<typename Derived>
auto TViewPathHandler<Derived>::onPathHitSurface(
	const std::size_t     pathLength,
	const SurfaceHit&     surfaceHit,
	const math::Spectrum& pathThroughput) -> ViewPathTracingPolicy
{
	return static_cast<Derived&>(*this).impl_onPathHitSurface(
		pathLength,
		surfaceHit, 
		pathThroughput);
}

template<typename Derived>
void TViewPathHandler<Derived>::onReceiverSampleEnd()
{
	static_cast<Derived&>(*this).impl_onReceiverSampleEnd();
}

template<typename Derived>
void TViewPathHandler<Derived>::onSampleBatchFinished()
{
	static_cast<Derived&>(*this).impl_onSampleBatchFinished();
}

}// end namespace ph
