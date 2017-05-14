#pragma once

#include "Actor/MotionSource/MotionSource.h"

namespace ph
{

class MDirectionalTranslation final : public MotionSource
{
public:
	virtual ~MDirectionalTranslation() override;

	virtual TDecomposedTransform<hiReal> genNextTransform(const TDecomposedTransform<hiReal>& current) const override;
};

}// end namespace ph