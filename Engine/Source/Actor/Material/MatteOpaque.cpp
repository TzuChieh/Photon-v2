#include "Actor/Material/MatteOpaque.h"
#include "Core/Texture/TConstantTexture.h"
#include "FileIO/SDL/InputPacket.h"
#include "Actor/Image/Image.h"
#include "Actor/Image/ConstantImage.h"
#include "FileIO/PictureLoader.h"
#include "Actor/Image/LdrPictureImage.h"
#include "Math/TVector3.h"
#include "Core/SurfaceBehavior/SurfaceOptics/LambertianDiffuse.h"
#include "Common/assertion.h"
#include "Core/SurfaceBehavior/SurfaceBehavior.h"

namespace ph
{

MatteOpaque::MatteOpaque() : 
	MatteOpaque(Vector3R(0.5_r))
{}

MatteOpaque::MatteOpaque(const Vector3R& linearSrgbAlbedo) : 
	SurfaceMaterial(),
	m_albedo()
{
	setAlbedo(linearSrgbAlbedo);
}

void MatteOpaque::genSurface(CookingContext& context, SurfaceBehavior& behavior) const
{
	PH_ASSERT(m_albedo);

	auto optics = std::make_shared<LambertianDiffuse>(
		m_albedo->genTextureSpectral(context));

	behavior.setOptics(optics);
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
	m_albedo()
{
	if(packet.hasReference<Image>("albedo"))
	{
		setAlbedo(packet.get<Image>("albedo"));
	}
	else if(packet.hasString("albedo"))
	{
		const Path& imagePath = packet.getStringAsPath("albedo", 
			Path(), DataTreatment::REQUIRED());

		setAlbedo(std::make_shared<LdrPictureImage>(PictureLoader::loadLdr(imagePath)));
	}
	else if(packet.hasVector3("albedo"))
	{
		setAlbedo(packet.getVector3("albedo"));
	}
	else if(packet.hasReal("albedo"))
	{
		setAlbedo(Vector3R(packet.getReal("albedo")));
	}
	else
	{
		std::cerr << "warning: at MatteOpaque ctor, "
		          << "ill-formed input detected, all albedo components are set to 0.5" << std::endl;
		setAlbedo(Vector3R(0.5_r));
	}

	PH_ASSERT(m_albedo);
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