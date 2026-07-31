#include "Engine/World/VisualWorld.h"
#include "Engine/Actor/Actor.h"
#include "Engine/Actor/Basic/exceptions.h"
#include "Engine/SDL/SceneDescription.h"
#include "Engine/World/Foundation/CookingContext.h"
#include "Engine/World/Foundation/TransientVisualElement.h"
#include "Engine/EngineEnv/CoreCookingContext.h"
#include "Engine/EngineEnv/sdl_accelerator_type.h"
#include "Engine/Core/Intersection/BruteForceIntersector.h"
#include "Engine/Core/Intersection/BVH/TBinaryBvhIntersector.h"
#include "Engine/Core/Intersection/BVH/TWideBvhIntersector.h"
#include "Engine/Core/Intersection/Intersector/TIndexedKdtreeIntersector.h"
#include "Engine/Core/Intersection/Kdtree/KdtreeIntersector.h"
#include "Engine/Core/Emitter/SurfaceEmitter.h"
#include "Engine/Core/Emitter/Sampler/ESPowerFavoring.h"
#include "Engine/Actor/Geometry/Geometry.h"
#include "Engine/Actor/Material/Material.h"
#include "Engine/Actor/MotionSource/MotionSource.h"
#include "Engine/World/Foundation/PreCookReport.h"
#include "Engine/World/Foundation/CommonCookingConfig.h"
#include "Engine/SDL/ISdlResource.h"
#include "Engine/SDL/SdlDependencyResolver.h"
#include "Engine/SDL/sdl_helpers.h"

#include <Common/assertion.h>
#include <Common/logging.h>
#include <Common/primitive_type.h>
#include <Common/profiling.h>
#include <Common/stats.h>

#include <limits>
#include <unordered_set>
#include <utility>
#include <vector>

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(VisualWorld, World);
PH_DEFINE_INTERNAL_TIMER_STAT(CookActors, VisualWorld);
PH_DEFINE_INTERNAL_TIMER_STAT(UpdateAccelerators, VisualWorld);
PH_DEFINE_INTERNAL_TIMER_STAT(UpdateLightSamplers, VisualWorld);

VisualWorld::VisualWorld()
	: m_cookedResources(nullptr)
	, m_cache(nullptr)
	, m_receiverPos(0)
	, m_rootActorsBound(math::Vector3R(0))
	, m_allActorsBound(math::Vector3R(0))
	, m_tlas(nullptr)
	//m_emitterSampler(std::make_shared<ESUniformRandom>()),
	, m_emitterSampler(std::make_unique<ESPowerFavoring>())
	, m_scene()
	, m_backgroundPrimitive(nullptr)
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

	std::vector<ResourceCookUnit> cookUnitStorage;
	{
		std::vector<std::string> resourceNames;
		std::vector<const ISdlResource*> resources = rawScene.getResources().listAll(&resourceNames);
		
		// Gather phantom resources
		std::unordered_set<const ISdlResource*> phantomResourceSet;
		{
			std::vector<std::string> phantomResourceNames;
			std::vector<const ISdlResource*> phantomResources = rawScene.getPhantoms().listAll(&phantomResourceNames);
			phantomResourceSet.insert(phantomResources.begin(), phantomResources.end());
			resources.insert(resources.end(), phantomResources.begin(), phantomResources.end());
			resourceNames.insert(resourceNames.end(), phantomResourceNames.begin(), phantomResourceNames.end());
		}

		// Record cook priorities
		constexpr std::size_t prioritiesPerLevel = std::numeric_limits<CookPriority>::max() + 1;
		std::vector<std::size_t> resourcePriorities(resources.size(), 0);
		for(std::size_t i = 0; i < resources.size(); ++i)
		{
			sdl::visit(resources[i],
				[&resourcePriorities, i](const Actor& actor)
				{
					const CookOrder order = actor.getCookOrder();
					// Level first, then in-level priorities
					resourcePriorities[i] = 1 + order.level * prioritiesPerLevel + order.priority;
				});
		}

		// Validate and reecord the complete dispatch order before producing any cooked data

		SdlDependencyResolver dependencyResolver;
		dependencyResolver.analyze(
			resources,
			{
				.resourceNames = resourceNames,
				.resourcePriorities = resourcePriorities
			});

		cookUnitStorage.reserve(resources.size());
		std::optional<CookLevel> previousActorCookLevel;
		while(const ISdlResource* resource = dependencyResolver.next())
		{
			ResourceCookUnit cookUnit;
			cookUnit.resource = resource;
			sdl::visit(resource,
				[&dependencyResolver, &previousActorCookLevel, &cookUnit, resource]
				(const Actor& actor)
				{
					const CookLevel actorCookLevel = actor.getCookOrder().level;
					if(previousActorCookLevel && actorCookLevel < *previousActorCookLevel)
					{
						// At this point should be engine implementation error
						throw_formatted<CookException>(
							"actor <{}> at cook level {} was dispatched after cook level {}; "
							"resource dependencies must not reverse actor cook levels",
							dependencyResolver.getResourceName(resource),
							static_cast<uint32>(actorCookLevel),
							static_cast<uint32>(*previousActorCookLevel));
					}

					previousActorCookLevel = actorCookLevel;
					cookUnit.actorCookLevel = actorCookLevel;
				});
			cookUnit.isPhantom = phantomResourceSet.contains(resource);
			cookUnitStorage.push_back(cookUnit);
		}
	}

	// Create the storage for cooked resources, and potentially free all previous resources
	m_cookedResources = std::make_unique<CookedResourceCollection>();

	// Cache should be freed already as it is not needed for rendering
	PH_ASSERT(m_cache == nullptr);
	m_cache = std::make_unique<TransientResourceCache>();

	// TODO: clear cooked data

	CookingContext ctx(this);
	CommonCookingConfig config;
	config.timeStep = coreCtx.getTimeStep();
	ctx.setCommonConfig(config);

	// Actor bounds always include the receiver position.
	// Actor bounds are published at the end of each actor cook level.
	m_rootActorsBound = math::AABB3D(m_receiverPos);
	m_allActorsBound = math::AABB3D(m_receiverPos);

	const TSpanView<ResourceCookUnit> cookUnitView = cookUnitStorage;
	std::optional<CookLevel> currentActorLevel;
	std::size_t currentActorLevelBegin = 0;
	for(std::size_t ri = 0; ri < cookUnitStorage.size(); ++ri)
	{
		const ResourceCookUnit& cookUnit = cookUnitView[ri];
		const std::optional<CookLevel>& actorCookLevel = cookUnit.actorCookLevel;
		if(actorCookLevel && (!currentActorLevel || *actorCookLevel > *currentActorLevel))
		{
			if(currentActorLevel)
			{
				onFinishedActorCookLevel(
					*currentActorLevel,
					cookUnitView.subspan(currentActorLevelBegin, ri - currentActorLevelBegin));
			}

			currentActorLevel = *actorCookLevel;
			currentActorLevelBegin = ri;
		}

		sdl::visit(cookUnit.resource,
			[&ctx](const Geometry& geometry)
			{
				const auto key = ctx.getKey(geometry);
				PH_ASSERT(!ctx.getResources().getGeometry(key));
				geometry.cook(ctx, *ctx.getResources().makeGeometry(key));
			},
			[&ctx](const Material& material)
			{
				const auto key = ctx.getKey(material);
				PH_ASSERT(!ctx.getResources().getMaterial(key));
				material.cook(ctx, *ctx.getResources().makeMaterial(key));
			},
			[&ctx](const MotionSource& motion)
			{
				const auto key = ctx.getKey(motion);
				PH_ASSERT(!ctx.getResources().getMotion(key));
				motion.cook(ctx, *ctx.getResources().makeMotion(key));
			},
			[this, &ctx, &cookUnitStorage, ri](const Actor& actor)
			{
				const TransientVisualElement* element = cookActor(actor, ctx);
				if(element && !cookUnitStorage[ri].isPhantom)
				{
					cookUnitStorage[ri].visibleElement = element;
				}
			});
	}
	if(currentActorLevel)
	{
		onFinishedActorCookLevel(
			*currentActorLevel,
			cookUnitView.subspan(currentActorLevelBegin));
	}

	// Gather cooked data for the top-level accelerator and emitter sampler
	std::vector<const Intersectable*> visibleIntersectables;
	std::vector<const Emitter*> visibleEmitters;
	for(const ResourceCookUnit& cookUnit : cookUnitView)
	{
		const TransientVisualElement* element = cookUnit.visibleElement;
		if(!element)
		{
			continue;
		}

		visibleIntersectables.insert(
			visibleIntersectables.end(),
			element->intersectables.begin(),
			element->intersectables.end());
		visibleEmitters.insert(
			visibleEmitters.end(),
			element->surfaceEmitters.begin(),
			element->surfaceEmitters.end());
	}

	m_backgroundPrimitive = m_cookedResources->getNamed().asConst()->getBackgroundPrimitive();

	PH_LOG(VisualWorld, Note, "discretized into {} visible intersectables, number of emitters: {}", 
		visibleIntersectables.size(), visibleEmitters.size());

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

		m_emitterSampler->update(visibleEmitters);
	}

	// Finished cooking

	PH_LOG(VisualWorld, Note,
		"done cooking actors, data cooked: {}", 
		getCookedResources()->getStats());

	// Clean up cache as it is not needed afterwards
	m_cache = nullptr;

	m_scene = std::make_unique<Scene>(m_tlas.get(), m_emitterSampler.get(), ctx.getCommonConfig().timeStep);
	m_scene->setBackgroundPrimitive(m_backgroundPrimitive);
}

const TransientVisualElement* VisualWorld::cookActor(const Actor& actor, CookingContext& ctx)
{
	PH_PROFILE_SCOPE();
	PH_SCOPED_TIMER(CookActors);

	// TODO: parallel preCook() and postCook()

	try
	{
		PreCookReport report = actor.preCook(ctx);
		if(!report.isCookable())
		{
			return nullptr;
		}

		TransientVisualElement element = actor.cook(ctx, report);
		actor.postCook(ctx, element);

		return m_cache->makeVisualElement(actor.getId(), std::move(element));
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

	return nullptr;
}

void VisualWorld::onFinishedActorCookLevel(CookLevel level, TSpanView<ResourceCookUnit> levelCookUnits)
{
	PH_PROFILE_SCOPE();

	// Accumulate visible actor bounds at the completed-level synchronization point
	math::AABB3D currentLevelActorsBound(m_receiverPos);
	for(const ResourceCookUnit& unit : levelCookUnits)
	{
		const TransientVisualElement* element = unit.visibleElement;
		if(!element)
		{
			continue;
		}

		for(const Intersectable* intersectable : element->intersectables)
		{
			currentLevelActorsBound.unionWith(intersectable->calcAABB());
		}
	}

	m_allActorsBound.unionWith(currentLevelActorsBound);
	if(level == static_cast<CookLevel>(ECookLevel::First))
	{
		m_rootActorsBound = m_allActorsBound;
	}

	PH_LOG(VisualWorld, Note,
		"finished actor cook level {}, cumulative bound: {}",
		static_cast<uint32>(level), m_allActorsBound.toString());
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

math::AABB3D VisualWorld::getAllActorsBound() const
{
	return m_allActorsBound;
}

}// end namespace ph
