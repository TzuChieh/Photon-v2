#include "Core/SurfaceBehavior/BsdfSampleQuery.h"
#include "Core/SurfaceBehavior/BsdfEvalQuery.h"

namespace ph
{

void BsdfSampleInput::set(const BsdfEvalQuery& eval)
{
	set(
		eval.inputs.X, 
		eval.inputs.V);
}

}// end namespace ph
