#include "Core/SurfaceBehavior/BsdfSample.h"
#include "Core/SurfaceBehavior/BsdfEvaluation.h"

namespace ph
{

void BsdfSampleInput::set(const BsdfEvaluation& bsdfEval)
{
	set(
		bsdfEval.inputs.X, 
		bsdfEval.inputs.V, 
		bsdfEval.inputs.elemental,
		bsdfEval.inputs.transported);
}

}// end namespace ph