#include "Actor/Light/AModelLight.h"
#include "Common/assertion.h"
#include "Common/logging.h"
#include "Core/Emitter/DiffuseSurfaceEmitter.h"
#include "Core/Emitter/MultiDiffuseSurfaceEmitter.h"
#include "World/Foundation/CookingContext.h"
#include "World/Foundation/CookedResourceCollection.h"

#include <utility>

namespace ph
{

std::shared_ptr<Geometry> AModelLight::getGeometry(const CookingContext& ctx) const
{
	return m_geometry;
}

std::shared_ptr<Material> AModelLight::getMaterial(const CookingContext& ctx) const
{
	return m_material;
}

const Emitter* AModelLight::buildEmitter(
	const CookingContext& ctx,
	TSpanView<const Primitive*> lightPrimitives) const
{
	if(lightPrimitives.empty())
	{
		PH_DEFAULT_LOG_ERROR(
			"failed building model light emitter: requires at least a light primitive");
		return nullptr;
	}

	PH_ASSERT(m_emittedRadiance);
	auto emittedRadiance = m_emittedRadiance->genColorTexture(ctx);

	const Emitter* lightEmitter = nullptr;
	if(lightPrimitives.size() == 1)
	{
		auto emitter = ctx.getResources()->makeEmitter<DiffuseSurfaceEmitter>(lightPrimitives[0]);
		emitter->setEmittedRadiance(emittedRadiance);

		if(m_isBackFaceEmit)
		{
			emitter->setBackFaceEmit();
		}
		else
		{
			emitter->setFrontFaceEmit();
		}

		lightEmitter = emitter;
	}
	else
	{
		PH_ASSERT_GT(lightPrimitives.size(), 1);

		std::vector<DiffuseSurfaceEmitter> primitiveEmitters;
		for(auto primitive : lightPrimitives)
		{
			DiffuseSurfaceEmitter emitter(primitive);
			emitter.setEmittedRadiance(emittedRadiance);
			primitiveEmitters.push_back(emitter);
		}

		auto multiEmitter = ctx.getResources()->makeEmitter<MultiDiffuseSurfaceEmitter>(
			std::move(primitiveEmitters));
		multiEmitter->setEmittedRadiance(emittedRadiance);

		if(m_isBackFaceEmit)
		{
			multiEmitter->setBackFaceEmit();
		}
		else
		{
			multiEmitter->setFrontFaceEmit();
		}

		lightEmitter = multiEmitter;
	}

	return lightEmitter;
}

void AModelLight::setGeometry(const std::shared_ptr<Geometry>& geometry)
{
	PH_ASSERT(geometry);
	m_geometry = geometry;
}

void AModelLight::setMaterial(const std::shared_ptr<Material>& material)
{
	// Material can be null
	m_material = material;
}

void AModelLight::setBackFaceEmit(bool isBackFaceEmit)
{
	m_isBackFaceEmit = isBackFaceEmit;
}

}// end namespace ph
