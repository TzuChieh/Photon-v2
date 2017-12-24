#include "Actor/Material/MatteOpaque.h"
#include "Core/Texture/TConstantTexture.h"
#include "FileIO/InputPacket.h"

namespace ph
{

MatteOpaque::MatteOpaque() : 
	Material(), 
	m_optics()
{
	
}

MatteOpaque::~MatteOpaque() = default;

void MatteOpaque::populateSurfaceBehavior(SurfaceBehavior* const out_surfaceBehavior) const
{
	out_surfaceBehavior->setSurfaceOptics(std::make_unique<LambertianDiffuse>(m_optics));
}

void MatteOpaque::setAlbedo(const Vector3R& albedo)
{
	setAlbedo(albedo.x, albedo.y, albedo.z);
}

void MatteOpaque::setAlbedo(const real r, const real g, const real b)
{
	SpectralStrength rgbSpectrum;
	rgbSpectrum.setRgb(Vector3R(r, g, b));
	setAlbedo(std::make_shared<TConstantTexture<SpectralStrength>>(rgbSpectrum));
}

void MatteOpaque::setAlbedo(const std::shared_ptr<TTexture<SpectralStrength>>& albedo)
{
	m_optics.setAlbedo(albedo);
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
	const Vector3R albedo = packet.getVector3r("albedo", Vector3R(0.5_r), 
	                                           DataTreatment::OPTIONAL("all components are set to 0.5"));

	std::unique_ptr<MatteOpaque> material = std::make_unique<MatteOpaque>();
	material->setAlbedo(albedo);
	return material;
}

}// end namespace ph