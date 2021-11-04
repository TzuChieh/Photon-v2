#include "Actor/LightSource/ModelSource.h"
#include "Actor/AModel.h"
#include "Core/Emitter/DiffuseSurfaceEmitter.h"
#include "Core/Texture/constant_textures.h"
#include "Core/Texture/LdrRgbTexture2D.h"
#include "Core/Texture/TextureLoader.h"
#include "Actor/LightSource/EmitterBuildingMaterial.h"
#include "Math/TVector3.h"
#include "Actor/Image/Image.h"
#include "Actor/Image/ConstantImage.h"
#include "Actor/Image/RasterFileImage.h"
#include "Common/assertion.h"
#include "Core/Emitter/MultiDiffuseSurfaceEmitter.h"
#include "Core/Intersectable/PrimitiveMetadata.h"
#include "Core/Intersectable/Primitive.h"
#include "Actor/Material/MatteOpaque.h"
#include "Actor/Geometry/Geometry.h"
#include "Common/logging.h"

#include <iostream>

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(ModelSource, Light);

ModelSource::ModelSource(const math::Vector3R& emittedRgbRadiance) :
	LightSource(), 
	m_emittedRadiance(nullptr),
	m_isBackFaceEmit(false)
{
	m_emittedRadiance = std::make_shared<ConstantImage>(
		emittedRgbRadiance, math::EColorSpace::Linear_sRGB);
}

ModelSource::ModelSource(const Path& imagePath) :
	LightSource(), 
	m_emittedRadiance(nullptr),
	m_isBackFaceEmit(false)
{
	m_emittedRadiance = std::make_shared<RasterFileImage>(imagePath);
}

ModelSource::ModelSource(const std::shared_ptr<Image>& emittedRadiance) :
	LightSource(),
	m_emittedRadiance(emittedRadiance),
	m_isBackFaceEmit(false)
{
	PH_ASSERT(m_emittedRadiance != nullptr);
}

std::unique_ptr<Emitter> ModelSource::genEmitter(
	ActorCookingContext& ctx, EmitterBuildingMaterial&& data) const
{
	if(data.primitives.empty())
	{
		PH_LOG_WARNING(ModelSource,
			"no primitive provided; requires at least a primitive to build emitter");
		return nullptr;
	}

	auto emittedRadiance = m_emittedRadiance->genColorTexture(ctx);

	std::vector<DiffuseSurfaceEmitter> primitiveEmitters;
	for(const auto& primitive : data.primitives)
	{
		DiffuseSurfaceEmitter emitter(primitive);
		emitter.setEmittedRadiance(emittedRadiance);
		primitiveEmitters.push_back(emitter);
	}

	std::unique_ptr<SurfaceEmitter> emitter;
	if(primitiveEmitters.size() == 1)
	{
		emitter = std::make_unique<DiffuseSurfaceEmitter>(primitiveEmitters[0]);
	}
	else
	{
		PH_ASSERT(!primitiveEmitters.empty());

		auto multiEmitter = std::make_unique<MultiDiffuseSurfaceEmitter>(std::move(primitiveEmitters));
		multiEmitter->setEmittedRadiance(emittedRadiance);
		emitter = std::move(multiEmitter);
	}

	PH_ASSERT(emitter != nullptr);
	if(m_isBackFaceEmit)
	{
		emitter->setBackFaceEmit();
	}
	else
	{
		emitter->setFrontFaceEmit();
	}

	return emitter;
}

std::shared_ptr<Geometry> ModelSource::genGeometry(ActorCookingContext& ctx) const
{
	return m_geometry;
}

std::shared_ptr<Material> ModelSource::genMaterial(ActorCookingContext& ctx) const
{
	return m_material;
}

void ModelSource::setGeometry(const std::shared_ptr<Geometry>& geometry)
{
	PH_ASSERT(geometry != nullptr);

	m_geometry = geometry;
}

void ModelSource::setMaterial(const std::shared_ptr<Material>& material)
{
	PH_ASSERT(material != nullptr);

	m_material = material;
}

void ModelSource::setBackFaceEmit(bool isBackFaceEmit)
{
	m_isBackFaceEmit = isBackFaceEmit;
}

}// end namespace ph
