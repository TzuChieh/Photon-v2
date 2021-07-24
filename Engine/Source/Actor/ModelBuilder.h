#pragma once

#include "Utility/IMoveOnly.h"
#include "Core/Intersectable/Intersectable.h"
#include "Core/Intersectable/PrimitiveMetadata.h"
#include "Math/Transform/Transform.h"
#include "Actor/CookedUnit.h"

#include <memory>

namespace ph
{

class ActorCookingContext;

class ModelBuilder final : private IMoveOnly
{
public:
	explicit ModelBuilder(ActorCookingContext& ctx);

	ModelBuilder& addIntersectable(std::unique_ptr<Intersectable> isable);

	// TODO: rename to set...
	ModelBuilder& addPrimitiveMetadata(std::unique_ptr<PrimitiveMetadata> metadata);

	ModelBuilder& transform(std::unique_ptr<math::Transform> LtoW, std::unique_ptr<math::Transform> WtoL);
	CookedUnit claimBuildResult();

private:
	ActorCookingContext& m_ctx;
	CookedUnit           m_cookedResults;
};

}// end namespace ph