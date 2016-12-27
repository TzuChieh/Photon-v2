#pragma once

#include "Core/SurfaceBehavior/SLight.h"
#include "Math/Vector3f.h"
#include "Core/Ray.h"
#include "Math/random_number.h"
#include "Math/constant.h"
#include "Core/Intersection.h"
#include "Core/SurfaceBehavior/SurfaceSample.h"

#include <iostream>

namespace ph
{

SLight::SLight() :
	m_emittedRadiance(0, 0, 0)
{

}

SLight::~SLight() = default;

void SLight::genImportanceSample(const Intersection& intersection, const Ray& ray, SurfaceSample* const out_sample) const
{
	// This lighting model is simple: the surface emits constant amount of radiance to any direction on
	// a specific point.

	out_sample->m_emittedRadiance = m_emittedRadiance;
	out_sample->m_type = ESurfaceSampleType::EMISSION;
}

}// end namespace ph