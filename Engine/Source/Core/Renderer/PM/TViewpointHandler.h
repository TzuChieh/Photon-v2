#pragma once

#include "Core/SurfaceHit.h"
#include "Math/TVector2.h"
#include "Core/Quantity/SpectralStrength.h"

#include <cstddef>

namespace ph
{

template<typename Derived>
class TViewpointHandler
{
	friend Derived;

public:
	bool onPathStart(
		const Vector2R&         filmNdc,
		const SpectralStrength& pathThroughput);

	bool onPathHitSurface(
		std::size_t             pathLength,
		const SurfaceHit&       surfaceHit,
		const SpectralStrength& pathThroughput);

	void onPathEnd(
		std::size_t             pathLength);

	void onSampleBatchFinished();

private:
	TViewpointHandler() = default;
	~TViewpointHandler() = default;
};

// In-header Implementations:

template<typename Derived>
bool TViewpointHandler<Derived>::onPathStart(
	const Vector2R&         filmNdc,
	const SpectralStrength& pathThroughput)
{
	return static_cast<Derived&>(*this).impl_onPathStart(
		filmNdc, 
		pathThroughput);
}

template<typename Derived>
bool TViewpointHandler<Derived>::onPathHitSurface(
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
void TViewpointHandler<Derived>::onPathEnd(
	const std::size_t       pathLength)
{
	static_cast<Derived&>(*this).impl_onPathEnd(
		pathLength);
}

template<typename Derived>
void TViewpointHandler<Derived>::onSampleBatchFinished()
{
	static_cast<Derived&>(*this).impl_onSampleBatchFinished();
}

}// end namespace ph