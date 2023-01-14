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

ModelSource::ModelSource() :
	LightSource(),
	m_emittedRadiance(std::make_shared<UnifiedColorImage>()),
	m_isBackFaceEmit(false)
{}

ModelSource::ModelSource(const math::Vector3R& emittedRgbRadiance) :
	ModelSource()
{
	m_emittedRadiance->setConstantColor(emittedRgbRadiance, math::EColorSpace::Linear_sRGB);
}

ModelSource::ModelSource(const Path& imagePath) :
	ModelSource()
{
	m_emittedRadiance->setImage(std::make_shared<RasterFileImage>(imagePath));
}

ModelSource::ModelSource(const std::shared_ptr<Image>& emittedRadiance) :
	ModelSource()
{
	m_emittedRadiance->setImage(emittedRadiance);
}

std::unique_ptr<Emitter> ModelSource::genEmitter(
	CookingContext& ctx, EmitterBuildingMaterial&& data) const
{
	if(data.primitives.empty())
	{
		PH_LOG_WARNING(ModelSource,
			"no primitive provided; requires at least a primitive to build emitter");
		return nullptr;
	}

	PH_ASSERT(m_emittedRadiance);
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

	PH_ASSERT(emitter);
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

std::shared_ptr<Geometry> ModelSource::genGeometry(CookingContext& ctx) const
{
	PH_ASSERT(m_geometry);
	return m_geometry;
}

std::shared_ptr<Material> ModelSource::genMaterial(CookingContext& ctx) const
{
	if(m_material)
	{
		return m_material;
	}
	else
	{
		return LightSource::genMaterial(ctx);
	}
}

void ModelSource::setGeometry(const std::shared_ptr<Geometry>& geometry)
{
	PH_ASSERT(geometry);
	m_geometry = geometry;
}

void ModelSource::setMaterial(const std::shared_ptr<Material>& material)
{
	// Material can be null
	m_material = material;
}

void ModelSource::setBackFaceEmit(bool isBackFaceEmit)
{
	m_isBackFaceEmit = isBackFaceEmit;
}

}// end namespace ph
