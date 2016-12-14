#include "Entity/Material/Integrand/SiPerfectMirror.h"
#include "Core/Ray.h"
#include "Math/Vector3f.h"
#include "Math/random_number.h"
#include "Math/constant.h"
#include "Core/Intersection.h"

#include <cmath>
#include <iostream>
#include <limits>

#define DIRAC_DELTA_HEIGHT_APPROXIMATION 1000.0f
#define REFLECTION_VECTOR_MARGIN 0.0005f

namespace ph
{

SiPerfectMirror::~SiPerfectMirror() = default;

void SiPerfectMirror::evaluateImportanceSample(const Intersection& intersection, const Ray& ray, SurfaceSample* const out_sample) const
{
	out_sample->m_type = ESurfaceSampleType::REFLECTION;
	out_sample->m_LiWeight.set(1.0f, 1.0f, 1.0f);
	out_sample->m_direction = ray.getDirection().reflect(intersection.getHitNormal());
}

}// end namespace ph