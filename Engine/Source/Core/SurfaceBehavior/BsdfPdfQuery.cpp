#include "Core/SurfaceBehavior/BsdfPdfQuery.h"
#include "Core/SurfaceBehavior/BsdfEvaluation.h"
#include "Core/SurfaceBehavior/BsdfSample.h"

namespace ph
{

void BsdfPdfQuery::Input::set(const BsdfEvaluation& bsdfEval)
{
	set(bsdfEval.inputs.X, bsdfEval.inputs.L, bsdfEval.inputs.V);
}

void BsdfPdfQuery::Input::set(const BsdfSample& sample)
{
	set(sample.inputs.X, sample.outputs.L, sample.inputs.V);
}

}// end namespace ph