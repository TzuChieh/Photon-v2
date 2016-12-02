#include "Model/Material/Integrand/SiTranslucentMicrofacet.h"

namespace ph
{

SiTranslucentMicrofacet::SiTranslucentMicrofacet()
{

}

SiTranslucentMicrofacet::~SiTranslucentMicrofacet() = default;

void SiTranslucentMicrofacet::genUniformRandomV(const Intersection& intersection, const Vector3f& L, Vector3f* out_V) const
{

}

void SiTranslucentMicrofacet::genImportanceRandomV(const Intersection& intersection, const Vector3f& L, Vector3f* out_V) const
{

}

void SiTranslucentMicrofacet::evaluateUniformRandomVPDF(const Intersection& intersection, const Vector3f& L, const Vector3f& V, Vector3f* const out_PDF) const
{

}

void SiTranslucentMicrofacet::evaluateImportanceRandomVPDF(const Intersection& intersection, const Vector3f& L, const Vector3f& V, Vector3f* const out_PDF) const
{

}

void SiTranslucentMicrofacet::evaluateLiWeight(const Intersection& intersection, const Vector3f& L, const Vector3f& V, Vector3f* const out_LiWeight) const
{

}

}// end namespace ph