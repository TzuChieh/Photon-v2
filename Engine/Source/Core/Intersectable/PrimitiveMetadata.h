#pragma once

#include "Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Math/Transform/StaticTransform.h"
#include "Core/UvwMapper/UvwMapper.h"

#include <memory>

namespace ph
{

class PrimitiveMetadata final
{
public:
	SurfaceBehavior            surfaceBehavior;
	std::shared_ptr<UvwMapper> uvwMapper;

	PrimitiveMetadata();
};

}// end namespace ph