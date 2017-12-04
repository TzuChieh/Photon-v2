#include "Actor/StGenerator/DefaultGenerator.h"
#include "Math/TVector3.h"

namespace ph
{

DefaultGenerator::~DefaultGenerator() = default;

void DefaultGenerator::map(const Vector3R& position, const Vector3R& uvw, Vector3R* const out_uvw) const
{
	*out_uvw = uvw;
}

}// end namespace ph