#include "World/VisualWorld.h"
#include "SDL/SceneDescription.h"
#include "World/Foundation/TransientVisualElement.h"
#include "World/Foundation/CookingContext.h"
#include "EngineEnv/CoreCookingContext.h"
#include "EngineEnv/CoreCookedUnit.h"
#include "EngineEnv/sdl_accelerator_type.h"
#include "Actor/Actor.h"
#include "Core/Intersection/BruteForceIntersector.h"
#include "Core/Intersection/BVH/TBinaryBvhIntersector.h"
#include "Core/Intersection/BVH/TWideBvhIntersector.h"
#include "Core/Intersection/Intersector/TIndexedKdtreeIntersector.h"
#include "Core/Intersection/Kdtree/KdtreeIntersector.h"
#include "Core/Emitter/Sampler/ESUniformRandom.h"
#include "Core/Emitter/Sampler/ESPowerFavoring.h"
#include "Actor/APhantomModel.h"
#include "World/Foundation/CookOrder.h"
#include "World/Foundation/PreCookReport.h"
#include "SDL/sdl_helpers.h"

#include <Common/primitive_type.h>
#include <Common/stats.h>
#include <Common/logging.h>
#include <Common/profiling.h>

#include <limits>
#include <iostream>
#include <algorithm>
#include <iterator>

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(VisualWorld, World);
PH_DEFINE_INTERNAL_TIMER_STAT(CookActorLevels, VisualWorld);
PH_DEFINE_INTERNAL_TIMER_STAT(UpdateAccelerators, VisualWorld);
PH_DEFINE_INTERNAL_TIMER_STAT(UpdateLightSamplers, VisualWorld);

VisualWorld::VisualWorld()
	: m_cookedResources(nullptr)
	, m_cache(nullptr)
	, m_tlas(nullptr)
	//m_emitterSampler(std::make_shared<ESUniformRandom>()),
	, m_emitterSampler(std::make_unique<ESPowerFavoring>())
	, m_scene()
	, m_receiverPos(0)
	, m_backgroundPrimitive(nullptr)
	, m_rootActorsBound(math::Vector3R(0))
	, m_leafActorsBound(math::Vector3R(0))
{}

//void VisualWorld::addActor(std::shared_ptr<Actor> actor)
//{
//	// TODO: allow duplicated actors?
//
//	if(actor != nullptr)
//	{
//		m_actors.push_back(actor);
//	}
//	else
//	{
//		std::cerr << "warning: at VisualWorld::addActor(), input is null" << std::endl;
//	}
//}

void VisualWorld::cook(const SceneDescription& rawScene, const CoreCookingContext& coreCtx)
{
	PH_PROFILE_SCOPE();
	PH_LOG(VisualWorld, Note, "started cooking...");

	// Create the storage for cooked resources, and potentially free all previous resources
	m_cookedResources = std::make_unique<CookedResourceCollection>();

	// Cache should be freed already as it is not needed for rendering
	PH_ASSERT(m_cache == nullptr);
	m_cache = std::make_unique<TransientResourceCache>();

	std::vector<SceneActor> sceneActors;
	for(auto actor : rawScene.getResources().getAllOfType<Actor>())
	{
		sceneActors.push_back({.actor = actor, .isPhantom = false});
	}
	for(auto phantomActor : rawScene.getPhantoms().getAllOfType<Actor>())
	{
		sceneActors.push_back({.actor = phantomActor, .isPhantom = true});
	}

	// TODO: clear cooked data

	CookingContext ctx(this);

	// TODO: should set to be receiver's bounds instead
	m_rootActorsBound = math::AABB3D(m_receiverPos);
	m_leafActorsBound = math::AABB3D(m_receiverPos);

	// Cook actors level by level (from lowest to highest)

	std::size_t numCookedActors = 0;
	std::vector<TransientVisualElement> visibleElements;
	while(numCookedActors < sceneActors.size())
	{
		PH_PROFILE_NAMED_SCOPE("Cook actors (single level)");
		PH_SCOPED_TIMER(CookActorLevels);

		auto actorCookBegin = sceneActors.begin() + numCookedActors;

		// Sort raw actors based on cook order
		std::sort(actorCookBegin, sceneActors.end(),
			[](const SceneActor& a, const SceneActor& b)
			{
				return a.actor->getCookOrder() < b.actor->getCookOrder();
			});

		const CookLevel currentActorLevel = actorCookBegin->actor->getCookOrder().level;
		PH_LOG(VisualWorld, Note, "cooking actor level: {}", currentActorLevel);

		// Find the transition point from current level to next level
		auto actorCookEnd = std::upper_bound(actorCookBegin, sceneActors.end(), currentActorLevel,
			[](const CookLevel a, SceneActor& b)
			{
				return a < b.actor->getCookOrder().level;
			});

		cookActors({actorCookBegin, actorCookEnd}, ctx, visibleElements);

		// Prepare for next cooking iteration

		// FIXME: calc bounds from newly cooked actors and union
		math::AABB3D bound = calcElementBound(visibleElements);
		// FIXME: should union with receiver's bound instead
		bound.unionWith(m_receiverPos);

		PH_LOG(VisualWorld, Note, "current iteration actor bound: {}", bound.toString());

		if(currentActorLevel == static_cast<CookLevel>(ECookLevel::First))
		{
			PH_LOG(VisualWorld, Note, "root actors bound calculated to be: {}", bound.toString());

			m_rootActorsBound = bound;
		}

		m_leafActorsBound = bound;
		numCookedActors += actorCookEnd - actorCookBegin;

		PH_LOG(VisualWorld, Note, "# cooked actors: {}", numCookedActors);
	}// end while more raw actors

	m_backgroundPrimitive = m_cookedResources->getNamed().asConst()->getBackgroundPrimitive();

	std::vector<const Intersectable*> visibleIntersectables;
	std::vector<const Emitter*> emitters;
	for(const TransientVisualElement& element : visibleElements)
	{
		for(const Intersectable* intersectable : element.intersectables)
		{
			visibleIntersectables.push_back(intersectable);
		}

		for(const Emitter* emitter : element.emitters)
		{
			emitters.push_back(emitter);
		}
	}

	PH_LOG(VisualWorld, Note, "discretized into {} visible intersectables, number of emitters: {}", 
		visibleIntersectables.size(), emitters.size());

	PH_LOG(VisualWorld, Note, "updating accelerator...");
	{
		PH_PROFILE_NAMED_SCOPE("Update accelerators");
		PH_SCOPED_TIMER(UpdateAccelerators);

		m_tlas = createTopLevelAccelerator(coreCtx.getTopLevelAcceleratorType(), visibleIntersectables);
		if(m_tlas)
		{
			m_tlas->update(visibleIntersectables);
		}
	}

	PH_LOG(VisualWorld, Note, "updating light sampler...");
	{
		PH_PROFILE_NAMED_SCOPE("Update light sampler");
		PH_SCOPED_TIMER(UpdateLightSamplers);

		m_emitterSampler->update(emitters);
	}

	// Finished cooking

	PH_LOG(VisualWorld, Note,
		"done cooking actors, data cooked: {}", 
		getCookedResources()->getStats());

	// Clean up cache as it is not needed afterwards
	m_cache = nullptr;

	m_scene = std::make_unique<Scene>(m_tlas.get(), m_emitterSampler.get());
	m_scene->setBackgroundPrimitive(m_backgroundPrimitive);
}

void VisualWorld::cookActors(
	TSpan<SceneActor> sceneActors,
	CookingContext& ctx,
	std::vector<TransientVisualElement>& out_elements)
{
	PH_PROFILE_SCOPE();

	// TODO: parallel preCook() and postCook()

	for(const SceneActor& sceneActor : sceneActors)
	{
		try
		{
			PreCookReport report = sceneActor.actor->preCook(ctx);
			TransientVisualElement element = sceneActor.actor->cook(ctx, report);
			sceneActor.actor->postCook(ctx, element);

			// Phantom actor is always cached
			if(sceneActor.isPhantom)
			{
				m_cache->makeVisualElement(sceneActor.actor->getId(), std::move(element));
			}
			// Normal actor is not cached
			else
			{
				out_elements.push_back(std::move(element));
			}
		}
		catch(const RuntimeException& e)
		{
			PH_LOG(VisualWorld, Error,
				"on cooking actor: {}", e.whatStr());
		}
		catch(const Exception& e)
		{
			PH_LOG(VisualWorld, Error,
				"on cooking actor: {}", e.what());
		}
	}
}

std::unique_ptr<Intersector> VisualWorld::createTopLevelAccelerator(
	EAccelerator acceleratorType,
	TSpanView<const Intersectable*> intersectables)
{
	if(acceleratorType == EAccelerator::Unspecified)
	{
		PH_DEBUG_LOG(VisualWorld,
			"TLAS unspecified, using BVH as default");

		acceleratorType = EAccelerator::BVH;
	}

	PH_LOG(VisualWorld, Note,
		"top-level acceleration structure (TLAS): {}",
		sdl::name_to_title_case(TSdlEnum<EAccelerator>{}[acceleratorType]));

	switch(acceleratorType)
	{
	case EAccelerator::BruteForce:
		return std::make_unique<BruteForceIntersector>();

	case EAccelerator::BVH:
		return std::make_unique<TBinaryBvhIntersector<std::size_t>>();

	case EAccelerator::BVH4:
		if(intersectables.size() <= std::numeric_limits<uint8>::max())
		{
			return std::make_unique<TWideBvhIntersector<4, uint8>>();
		}
		else if(intersectables.size() <= std::numeric_limits<uint16>::max())
		{
			return std::make_unique<TWideBvhIntersector<4, uint16>>();
		}
		else if(intersectables.size() <= std::numeric_limits<uint32>::max())
		{
			return std::make_unique<TWideBvhIntersector<4, uint32>>();
		}
		else if(intersectables.size() <= std::numeric_limits<uint64>::max())
		{
			return std::make_unique<TWideBvhIntersector<4, uint64>>();
		}
		else
		{
			PH_ASSERT_UNREACHABLE_SECTION();
			return nullptr;
		}

	case EAccelerator::BVH8:
		if(intersectables.size() <= std::numeric_limits<uint8>::max())
		{
			return std::make_unique<TWideBvhIntersector<8, uint8>>();
		}
		else if(intersectables.size() <= std::numeric_limits<uint16>::max())
		{
			return std::make_unique<TWideBvhIntersector<8, uint16>>();
		}
		else if(intersectables.size() <= std::numeric_limits<uint32>::max())
		{
			return std::make_unique<TWideBvhIntersector<8, uint32>>();
		}
		else if(intersectables.size() <= std::numeric_limits<uint64>::max())
		{
			return std::make_unique<TWideBvhIntersector<8, uint64>>();
		}
		else
		{
			PH_ASSERT_UNREACHABLE_SECTION();
			return nullptr;
		}

	case EAccelerator::Kdtree:
		return std::make_unique<KdtreeIntersector>();

	case EAccelerator::IndexedKdtree:
		if(intersectables.size() <= std::numeric_limits<uint8>::max())
		{
			return std::make_unique<TIndexedKdtreeIntersector<uint8>>();
		}
		else if(intersectables.size() <= std::numeric_limits<uint16>::max())
		{
			return std::make_unique<TIndexedKdtreeIntersector<uint16>>();
		}
		else if(intersectables.size() <= std::numeric_limits<uint32>::max())
		{
			return std::make_unique<TIndexedKdtreeIntersector<uint32>>();
		}
		else if(intersectables.size() <= std::numeric_limits<uint64>::max())
		{
			return std::make_unique<TIndexedKdtreeIntersector<uint64>>();
		}
		else
		{
			PH_ASSERT_UNREACHABLE_SECTION();
			return nullptr;
		}

	default:
		PH_ASSERT_UNREACHABLE_SECTION();
		return nullptr;
	}
}

math::AABB3D VisualWorld::calcElementBound(TSpanView<TransientVisualElement> elements)
{
	PH_PROFILE_SCOPE();

	std::vector<const Intersectable*> intersectables;
	for(const TransientVisualElement& element : elements)
	{
		for(const Intersectable* intersectable : element.intersectables)
		{
			intersectables.push_back(intersectable);
		}
	}

	if(intersectables.size() == 0)
	{
		return math::AABB3D();
	}

	math::AABB3D fullBound = intersectables.front()->calcAABB();
	for(const Intersectable* intersectable : intersectables)
	{
		fullBound.unionWith(intersectable->calcAABB());
	}
	return fullBound;
}

CookedResourceCollection* VisualWorld::getCookedResources() const
{
	return m_cookedResources.get();
}

TransientResourceCache* VisualWorld::getCache() const
{
	return m_cache.get();
}

math::AABB3D VisualWorld::getRootActorsBound() const
{
	return m_rootActorsBound;
}

math::AABB3D VisualWorld::getLeafActorsBound() const
{
	return m_leafActorsBound;
}

}// end namespace ph
