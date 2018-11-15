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
	void onPathStart(
		const Vector2R&         filmNdc,
		const SpectralStrength& pathThroughput);

	void onPathSurfaceHit(
		std::size_t             nthSurfaceHit,
		const SurfaceHit&       surfaceHit,
		const SpectralStrength& pathThroughput);

	void onPathEnd(
		std::size_t             numHits);

private:
	TViewpointListener() = default;
	~TViewpointListener() = default;
};

// In-header Implementations:

template<typename Derived>
void TViewpointHandler<Derived>::onPathStart(
	const Vector2R&         filmNdc,
	const SpectralStrength& pathThroughput)
{
	static_cast<Derived&>(*this).impl_onPathStart(
		filmNdc, 
		pathThroughput);
}

template<typename Derived>
void TViewpointHandler<Derived>::onPathSurfaceHit(
	const std::size_t       nthSurfaceHit,
	const SurfaceHit&       surfaceHit,
	const SpectralStrength& pathThroughput)
{
	static_cast<Derived&>(*this).impl_onPathSurfaceHit(
		nthSurfaceHit, 
		surfaceHit, 
		pathThroughput);
}

template<typename Derived>
void TViewpointHandler<Derived>::onPathEnd(
	const std::size_t       numHits)
{
	static_cast<Derived&>(*this).impl_onPathEnd(
		numHits);
}

}// end namespace ph