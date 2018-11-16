#pragma once

#include "Core/SurfaceHit.h"
#include "Math/TVector2.h"
#include "Core/Quantity/SpectralStrength.h"
#include "Core/Renderer/PM/ViewPathTracingPolicy.h"

#include <cstddef>

namespace ph
{

template<typename Derived>
class TViewPathHandler
{
	friend Derived;

public:
	bool onCameraSampleStart(
		const Vector2R&         filmNdc,
		const SpectralStrength& pathThroughput);

	ViewPathTracingPolicy onPathHitSurface(
		std::size_t             pathLength,
		const SurfaceHit&       surfaceHit,
		const SpectralStrength& pathThroughput);

	void onCameraSampleEnd();

	void onSampleBatchFinished();

private:
	TViewPathHandler() = default;
	~TViewPathHandler() = default;
};

// In-header Implementations:

template<typename Derived>
bool TViewPathHandler<Derived>::onCameraSampleStart(
	const Vector2R&         filmNdc,
	const SpectralStrength& pathThroughput)
{
	return static_cast<Derived&>(*this).impl_onCameraSampleStart(
		filmNdc, 
		pathThroughput);
}

template<typename Derived>
ViewPathTracingPolicy TViewPathHandler<Derived>::onPathHitSurface(
	const std::size_t       pathLength,
	const SurfaceHit&       surfaceHit,
	const SpectralStrength& pathThroughput)
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