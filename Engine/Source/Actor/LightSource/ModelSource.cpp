#include "Actor/LightSource/ModelSource.h"
#include "Actor/AModel.h"
#include "Core/Emitter/DiffuseSurfaceEmitter.h"
#include "Core/Texture/TConstantTexture.h"
#include "Core/Texture/LdrRgbTexture2D.h"
#include "Core/Texture/TextureLoader.h"
#include "FileIO/SDL/InputPacket.h"
#include "Actor/LightSource/EmitterBuildingMaterial.h"
#include "Math/TVector3.h"
#include "FileIO/SDL/InputPrototype.h"
#include "FileIO/PictureLoader.h"
#include "Actor/Image/Image.h"
#include "Actor/Image/ConstantImage.h"
#include "Actor/Image/LdrPictureImage.h"
#include "Common/assertion.h"
#include "Core/Emitter/MultiDiffuseSurfaceEmitter.h"
#include "Core/Intersectable/PrimitiveMetadata.h"
#include "Core/Intersectable/Primitive.h"
#include "Actor/Material/MatteOpaque.h"
#include "Actor/Geometry/Geometry.h"

#include <iostream>

namespace ph
{

const Logger ModelSource::logger(LogSender("Model Source"));

ModelSource::ModelSource(const Vector3R& emittedRgbRadiance) :
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

ModelSource::~ModelSource() = default;

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

SdlTypeInfo ModelSource::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_LIGHT_SOURCE, "model");
}

void ModelSource::ciRegister(CommandRegister& cmdRegister)
{
	SdlLoader loader;
	loader.setFunc<ModelSource>(ciLoad);
	cmdRegister.setLoader(loader);
}

std::unique_ptr<ModelSource> ModelSource::ciLoad(const InputPacket& packet)
{
	std::unique_ptr<ModelSource> source;
	if(packet.hasReference<Image>("emitted-radiance"))
	{
		source = std::make_unique<ModelSource>(packet.get<Image>("emitted-radiance"));
	}
	else if(packet.hasString("emitted-radiance"))
	{
		source = std::make_unique<ModelSource>(packet.getStringAsPath("emitted-radiance"));
	}
	else
	{
		const auto& emittedRadiance = packet.getVector3("emitted-radiance", 
			Vector3R(0), DataTreatment::REQUIRED());

		source = std::make_unique<ModelSource>(emittedRadiance);
	}
	PH_ASSERT(source != nullptr);

	auto geometry = packet.get<Geometry>("geometry", DataTreatment::REQUIRED());
	auto material = packet.get<Material>("material", DataTreatment::OPTIONAL());
	if(material == nullptr)
	{
		logger.log(ELogLevel::NOTE_MED, 
		           "material not specified, using diffusive material as default");
		material = std::make_shared<MatteOpaque>(Vector3R(0.5_r));
	}

	source->setGeometry(geometry);
	source->setMaterial(material);

	const auto& emitMode = packet.getString("emit-mode", "front");
	if(emitMode == "front")
	{
		source->setBackFaceEmit(false);
	}
	else if(emitMode == "back")
	{
		source->setBackFaceEmit(true);
	}

	return source;
}

}// end namespace ph