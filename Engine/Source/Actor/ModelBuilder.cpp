#include "Actor/ModelBuilder.h"
#include "Actor/CookedUnit.h"
#include "Actor/CookingContext.h"
#include "Core/Intersectable/TransformedIntersectable.h"

namespace ph
{

ModelBuilder::ModelBuilder(CookingContext& context) :
	m_context(context), m_cookedResults()
{}

ModelBuilder& ModelBuilder::addIntersectable(std::unique_ptr<Intersectable> isable)
{
	m_cookedResults.addIntersectable(std::move(isable));

	return *this;
}

ModelBuilder& ModelBuilder::addPrimitiveMetadata(std::unique_ptr<PrimitiveMetadata> metadata)
{
	m_cookedResults.setPrimitiveMetadata(std::move(metadata));

	return *this;
}

ModelBuilder& ModelBuilder::transform(std::unique_ptr<Transform> LtoW,
                                      std::unique_ptr<Transform> WtoL)
{
	for(auto& intersectable : m_cookedResults.intersectables())
	{
		auto oldIsable = std::move(intersectable);
		auto newIsable = std::make_unique<TransformedIntersectable>(oldIsable.get(), 
		                                                            LtoW.get(),
		                                                            WtoL.get());
		m_context.addBackend(std::move(oldIsable));
		intersectable = std::move(newIsable);
	}

	m_cookedResults.addTransform(std::move(LtoW));
	m_cookedResults.addTransform(std::move(WtoL));

	return *this;
}

CookedUnit ModelBuilder::claimBuildResult()
{
	return std::move(m_cookedResults);
}

}// end namespace ph