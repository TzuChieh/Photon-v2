#pragma once

#include "Core/SurfaceBehavior/SurfaceOptics.h"

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

	ESurfacePhenomenon getPhenomenonOf(SurfaceElemental elemental) const override;

private:
	void calcBsdf(
		const BsdfQueryContext& ctx,
		const BsdfEvalInput&    in,
		BsdfEvalOutput&         out) const override;

	void genBsdfSample(
		const BsdfQueryContext& ctx,
		const BsdfSampleInput&  in,
		SampleFlow&             sampleFlow,
		BsdfSampleOutput&       out) const override;

	void calcBsdfPdf(
		const BsdfQueryContext& ctx,
		const BsdfPdfInput&     in,
		BsdfPdfOutput&          out) const override;
};

}// end namespace ph
