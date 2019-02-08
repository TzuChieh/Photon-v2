#pragma once

#include "Math/math_fwd.h"

namespace ph
{

class Ray;

template<typename Derived>
class TSensedRayProcessor
{
	friend Derived;

public:
	void process(const Vector2D& sensorNdc, const Ray& ray);

private:
	TSensedRayProcessor() = default;
	~TSensedRayProcessor() = default;
};

// In-header Implementations:

template<typename Derived>
inline void TSensedRayProcessor<Derived>::process(
	const Vector2D& sensorNdc, 
	const Ray&      ray)
{
	static_cast<Derived&>(*this).impl_process(sensorNdc, ray);
}

}// end namespace ph