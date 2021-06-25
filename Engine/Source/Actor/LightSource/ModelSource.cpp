#include "Actor/LightSource/ModelSource.h"
#include "Actor/AModel.h"
#include "Core/Emitter/DiffuseSurfaceEmitter.h"
#include "Core/Texture/TConstantTexture.h"
#include "Core/Texture/LdrRgbTexture2D.h"
#include "Core/Texture/TextureLoader.h"
#include "Actor/LightSource/EmitterBuildingMaterial.h"
#include "Math/TVector3.h"
#include "DataIO/PictureLoader.h"
#include "Actor/Image/Image.h"
#include "Actor/Image/ConstantImage.h"
#include "Actor/Image/LdrPictureImage.h"
#include "Common/assertion.h"
#include "Core/Emitter/MultiDiffuseSurfaceEmitter.h"
#include "Core/Intersectable/PrimitiveMetadata.h"
#include "Core/Intersectable/Primitive.h"
#include "Actor/Material/MatteOpaque.h"
#include "Actor/Geometry/Geometry.h"
#include "Common/Logger.h"

#include <iostream>

namespace ph
{

namespace
{

const Logger logger(LogSender("Model Source"));

}

ModelSource::ModelSource(const math::Vector3R& emittedRgbRadiance) :
	LightSource(), 
	m_emittedRadiance(nullptr),
	m_isBackFaceEmit(false)
{
	m_emittedRadiance = std::make_shared<ConstantImage>(emittedRgbRadiance, 
	                                                    ConstantImage::EType::EMR_LINEAR_SRGB);
}

ModelSource::ModelSource(const Path& imagePath) :
	LightSource(), 
	m_emittedRadiance(nullptr),
	m_isBackFaceEmit(false)
{
	auto image = std::make_shared<LdrPictureImage>(PictureLoader::loadLdr(imagePath));
	m_emittedRadiance = image;
}

ModelSource::ModelSource(const std::shared_ptr<Image>& emittedRadiance) :
	LightSource(),
	m_emittedRadiance(emittedRadiance),
	m_isBackFaceEmit(false)
{
	PH_ASSERT(m_emittedRadiance != nullptr);
}

std::unique_ptr<Emitter> ModelSource::genEmitter(
	CookingContext& context, EmitterBuildingMaterial&& data) const
{
	if(data.primitives.empty())
	{
		logger.log(ELogLevel::WARNING_MED, 
		           "no primitive provided; requires at least a primitive to build emitter");
		return nullptr;
	}

	auto emittedRadiance = m_emittedRadiance->genTextureSpectral(context);

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

std::shared_ptr<Geometry> ModelSource::genGeometry(CookingContext& context) const
{
	return m_geometry;
}

std::shared_ptr<Material> ModelSource::genMaterial(CookingContext& context) const
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
