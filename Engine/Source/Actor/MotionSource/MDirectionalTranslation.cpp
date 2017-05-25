#include "Actor/MotionSource/MDirectionalTranslation.h"

namespace ph
{

MDirectionalTranslation::~MDirectionalTranslation() = default;

TDecomposedTransform<hiReal> MDirectionalTranslation::genNextTransform(const TDecomposedTransform<hiReal>& current) const
{
	return TDecomposedTransform<hiReal>(current);
}

}// end namespace ph