#pragma once

#include "Core/SurfaceBehavior/SurfaceOptics.h"

#include <memory>

namespace ph
{

/*
	An ideal absorber absorbs any incoming energy. Pretty much resembles
	a black hole under event horizon.
*/
class IdealAbsorber : public SurfaceOptics
{
public:
	IdealAbsorber();

private:
	void calcBsdf(
		const BsdfEvaluation::Input& in,
		BsdfEvaluation::Output&      out,
		const SidednessAgreement&    sidedness) const override;

	void calcBsdfSample(
		const BsdfSample::Input&     in,
		BsdfSample::Output&          out,
		const SidednessAgreement&    sidedness) const override;

	void calcBsdfSamplePdfW(
		const BsdfPdfQuery::Input&   in,
		BsdfPdfQuery::Output&        out,
		const SidednessAgreement&    sidedness) const override;
};

}// end namespace ph