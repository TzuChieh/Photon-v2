#include "Actor/ModelBuilder.h"
#include "World/Foundation/TransientVisualElement.h"
#include "World/Foundation/CookingContext.h"
#include "Core/Intersection/TransformedIntersectable.h"

namespace ph
{

//ModelBuilder::ModelBuilder(CookingContext& ctx) :
//	m_ctx(ctx), m_cookedResults()
//{}
//
//ModelBuilder& ModelBuilder::addIntersectable(std::unique_ptr<Intersectable> isable)
//{
//	m_cookedResults.addIntersectable(std::move(isable));
//
//	return *this;
//}
//
//ModelBuilder& ModelBuilder::addPrimitiveMetadata(std::unique_ptr<PrimitiveMetadata> metadata)
//{
//	m_cookedResults.setPrimitiveMetadata(std::move(metadata));
//
//	return *this;
//}
//
//ModelBuilder& ModelBuilder::transform(
//	std::unique_ptr<math::Transform> LtoW,
//	std::unique_ptr<math::Transform> WtoL)
//{
//	for(auto& intersectable : m_cookedResults.intersectables())
//	{
//		auto oldIsable = std::move(intersectable);
//		auto newIsable = std::make_unique<TransformedIntersectable>(oldIsable.get(), 
//		                                                            LtoW.get(),
//		                                                            WtoL.get());
//		m_cookedResults.addBackend(std::move(oldIsable));
//		intersectable = std::move(newIsable);
//	}
//
//	m_cookedResults.addTransform(std::move(LtoW));
//	m_cookedResults.addTransform(std::move(WtoL));
//
//	return *this;
//}
//
//CookedUnit ModelBuilder::claimBuildResult()
//{
//	return std::move(m_cookedResults);
//}

}// end namespace ph
