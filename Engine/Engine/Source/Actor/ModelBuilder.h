#pragma once

#include "Utility/IMoveOnly.h"
#include "Core/Intersectable/Intersectable.h"
#include "Core/Intersectable/PrimitiveMetadata.h"
#include "Math/Transform/Transform.h"
#include "World/Foundation/TransientVisualElement.h"

#include <memory>

namespace ph
{

//class CookingContext;
//
//class ModelBuilder final : private IMoveOnly
//{
//public:
//	explicit ModelBuilder(CookingContext& ctx);
//
//	ModelBuilder& addIntersectable(std::unique_ptr<Intersectable> isable);
//
//	// TODO: rename to set...
//	ModelBuilder& addPrimitiveMetadata(std::unique_ptr<PrimitiveMetadata> metadata);
//
//	ModelBuilder& transform(std::unique_ptr<math::Transform> LtoW, std::unique_ptr<math::Transform> WtoL);
//	CookedUnit claimBuildResult();
//
//private:
//	CookingContext& m_ctx;
//	CookedUnit m_cookedResults;
//};

}// end namespace ph