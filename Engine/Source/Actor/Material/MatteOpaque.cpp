#include "Actor/Material/MatteOpaque.h"
#include "Core/Texture/TConstantTexture.h"
#include "FileIO/InputPacket.h"
#include "FileIO/InputPrototype.h"
#include "Actor/Image/Image.h"
#include "Actor/Image/ConstantImage.h"
#include "FileIO/PictureLoader.h"
#include "Actor/Image/LdrPictureImage.h"

namespace ph
{

MatteOpaque::MatteOpaque() : 
	Material(), 
	m_albedo(std::make_shared<ConstantImage>(Vector3R(0.5_r)))
{
	
}

MatteOpaque::~MatteOpaque() = default;

void MatteOpaque::populateSurfaceBehavior(CookingContext& context, SurfaceBehavior* const out_surfaceBehavior) const
{
	std::shared_ptr<TTexture<SpectralStrength>> albedoTexture;
	m_albedo->genTextureSpectral(context, &albedoTexture);

	std::unique_ptr<LambertianDiffuse> surfaceOptics = std::make_unique<LambertianDiffuse>();
	surfaceOptics->setAlbedo(albedoTexture);
	out_surfaceBehavior->setSurfaceOptics(std::move(surfaceOptics));
}

void MatteOpaque::setAlbedo(const Vector3R& albedo)
{
	setAlbedo(albedo.x, albedo.y, albedo.z);
}

void MatteOpaque::setAlbedo(const real r, const real g, const real b)
{
	m_albedo = std::make_shared<ConstantImage>(std::vector<real>{r, g, b});
}

void MatteOpaque::setAlbedo(const std::shared_ptr<Image>& albedo)
{
	m_albedo = albedo;
}

// command interface

SdlTypeInfo MatteOpaque::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_MATERIAL, "matte-opaque");
}

void MatteOpaque::ciRegister(CommandRegister& cmdRegister)
{
	SdlLoader loader;
	loader.setFunc<MatteOpaque>(ciLoad);
	cmdRegister.setLoader(loader);
}

std::unique_ptr<MatteOpaque> MatteOpaque::ciLoad(const InputPacket& packet)
{
	InputPrototype imageAlbedo;
	imageAlbedo.addString("albedo");

	InputPrototype constAlbedo;
	constAlbedo.addVector3r("albedo");

	std::unique_ptr<MatteOpaque> material = std::make_unique<MatteOpaque>();
	if(packet.isPrototypeMatched(imageAlbedo))
	{
		const Path& imagePath = packet.getStringAsPath("albedo", Path(), DataTreatment::REQUIRED());
		const auto& albedo    = std::make_shared<LdrPictureImage>(PictureLoader::loadLdr(imagePath));
		material->setAlbedo(albedo);
	}
	else if(packet.isPrototypeMatched(constAlbedo))
	{
		const Vector3R albedo = packet.getVector3r("albedo", Vector3R(0.5_r), 
			DataTreatment::OPTIONAL("all components are set to 0.5"));
		material->setAlbedo(albedo);
	}
	else
	{
		std::cerr << "warning: at MatteOpaque::ciLoad(), " 
		          << "ill-formed input detected" << std::endl;
	}
	
	return material;
}

}// end namespace ph