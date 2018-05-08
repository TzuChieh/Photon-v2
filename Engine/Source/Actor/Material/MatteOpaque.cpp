#include "Actor/Material/MatteOpaque.h"
#include "Core/Texture/TConstantTexture.h"
#include "FileIO/SDL/InputPacket.h"
#include "FileIO/SDL/InputPrototype.h"
#include "Actor/Image/Image.h"
#include "Actor/Image/ConstantImage.h"
#include "FileIO/PictureLoader.h"
#include "Actor/Image/LdrPictureImage.h"
#include "Math/TVector3.h"
#include "Core/SurfaceBehavior/SurfaceOptics/LambertianDiffuse.h"

namespace ph
{

MatteOpaque::MatteOpaque() : 
	MatteOpaque(Vector3R(0.5_r))
{}

MatteOpaque::MatteOpaque(const Vector3R& linearSrgbAlbedo) : 
	SurfaceMaterial(),
	m_albedo(nullptr)
{
	setAlbedo(linearSrgbAlbedo);
}

MatteOpaque::~MatteOpaque() = default;

std::shared_ptr<SurfaceOptics> MatteOpaque::genSurfaceOptics(CookingContext& context) const
{
	auto surfaceOptics = std::make_shared<LambertianDiffuse>();
	surfaceOptics->setAlbedo(m_albedo->genTextureSpectral(context));
	return surfaceOptics;
}

void MatteOpaque::setAlbedo(const Vector3R& albedo)
{
	setAlbedo(albedo.x, albedo.y, albedo.z);
}

void MatteOpaque::setAlbedo(const real r, const real g, const real b)
{
	m_albedo = std::make_shared<ConstantImage>(std::vector<real>{r, g, b}, ConstantImage::EType::ECF_LINEAR_SRGB);
}

void MatteOpaque::setAlbedo(const std::shared_ptr<Image>& albedo)
{
	m_albedo = albedo;
}

// command interface

MatteOpaque::MatteOpaque(const InputPacket& packet) :
	SurfaceMaterial(packet),
	m_albedo(nullptr)
{
	InputPrototype imageFileAlbedo;
	imageFileAlbedo.addString("albedo");

	InputPrototype constAlbedo;
	constAlbedo.addVector3r("albedo");

	if(packet.isPrototypeMatched(imageFileAlbedo))
	{
		const Path& imagePath = packet.getStringAsPath("albedo", Path(), DataTreatment::REQUIRED());
		setAlbedo(std::make_shared<LdrPictureImage>(PictureLoader::loadLdr(imagePath)));
	}
	else if(packet.isPrototypeMatched(constAlbedo))
	{
		const Vector3R albedo = packet.getVector3r("albedo", Vector3R(0.5_r), 
			DataTreatment::OPTIONAL("all components are set to 0.5"));
		setAlbedo(albedo);
	}
	else
	{
		const auto& albedo = packet.get<Image>("albedo");
		if(albedo != nullptr)
		{
			setAlbedo(albedo);
		}
		else
		{
			std::cerr << "warning: at MatteOpaque::ciLoad(), " 
		              << "ill-formed input detected" << std::endl;
		}
	}
}

SdlTypeInfo MatteOpaque::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_MATERIAL, "matte-opaque");
}

void MatteOpaque::ciRegister(CommandRegister& cmdRegister)
{
	SdlLoader loader;
	loader.setFunc<MatteOpaque>([](const InputPacket& packet)
	{
		return std::make_unique<MatteOpaque>(packet);
	});
	cmdRegister.setLoader(loader);
}

}// end namespace ph