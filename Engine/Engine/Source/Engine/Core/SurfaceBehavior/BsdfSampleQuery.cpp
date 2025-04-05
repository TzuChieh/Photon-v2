#include "Engine/Core/SurfaceBehavior/BsdfSampleQuery.h"
#include "Engine/Core/SurfaceBehavior/BsdfEvalQuery.h"

namespace ph
{

void BsdfSampleInput::set(const BsdfEvalInput& evalInput)
{
	set(
		evalInput.getX(),
		evalInput.getV());
}

}// end namespace ph
