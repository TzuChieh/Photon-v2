#include "Core/SurfaceBehavior/BsdfSample.h"
#include "Core/SurfaceBehavior/BsdfEvaluation.h"

namespace ph
{

void BsdfSample::Input::set(const BsdfEvaluation& bsdfEval)
{
	set(
		bsdfEval.inputs.X, 
		bsdfEval.inputs.V, 
		bsdfEval.inputs.elemental);
}

}// end namespace ph