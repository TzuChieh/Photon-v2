#include "Camera/DefaultCamera.h"

#include "Core/Ray.h"

namespace ph
{

DefaultCamera::DefaultCamera() :
	Camera()
{

}

DefaultCamera::~DefaultCamera() = default;

void DefaultCamera::genSampleRay(Ray* const out_ray, const uint32 xRes, const uint32 yRes, float32 x, float32 y) const
{

}

}// end namespace ph