#pragma once

#include "Core/SurfaceHit.h"
#include "Math/TVector2.h"
#include "Core/Quantity/SpectralStrength.h"
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
	// Called after a camera sample is generated.
	// Returns whether this camera sample should be used.
	bool onCameraSampleStart(
		const math::Vector2R&   filmNdc,
		const SpectralStrength& pathThroughput);

	// Called after the view path hits a surface, corresponding hit information
	// is given.
	// Returns a policy for controlling how to trace the next path.
	auto onPathHitSurface(
		std::size_t             pathLength,
		const SurfaceHit&       surfaceHit,
		const SpectralStrength& pathThroughput) -> ViewPathTracingPolicy;

	// Called after a camera sample is ended.
	void onCameraSampleEnd();

	// Called after a batch of camera samples has been consumed.
	void onSampleBatchFinished();

private:
	TViewPathHandler() = default;
	~TViewPathHandler() = default;
};

// In-header Implementations:

template<typename Derived>
bool TViewPathHandler<Derived>::onCameraSampleStart(
	const math::Vector2R&   filmNdc,
	const SpectralStrength& pathThroughput)
{
	return static_cast<Derived&>(*this).impl_onCameraSampleStart(
		filmNdc, 
		pathThroughput);
}

template<typename Derived>
auto TViewPathHandler<Derived>::onPathHitSurface(
	const std::size_t       pathLength,
	const SurfaceHit&       surfaceHit,
	const SpectralStrength& pathThroughput) -> ViewPathTracingPolicy
{
	return static_cast<Derived&>(*this).impl_onPathHitSurface(
		pathLength,
		surfaceHit, 
		pathThroughput);
}

template<typename Derived>
void TViewPathHandler<Derived>::onCameraSampleEnd()
{
	static_cast<Derived&>(*this).impl_onCameraSampleEnd();
}

template<typename Derived>
void TViewPathHandler<Derived>::onSampleBatchFinished()
{
	static_cast<Derived&>(*this).impl_onSampleBatchFinished();
}

}// end namespace ph
