#include "Actor/Material/MatteOpaque.h"
#include "Actor/Texture/ConstantTexture.h"
#include "FileIO/InputPacket.h"

namespace ph
{

MatteOpaque::MatteOpaque() : 
	Material(), 
	m_bsdf()
{
	
}

MatteOpaque::MatteOpaque(const InputPacket& packet) :
	Material(packet), 
	m_bsdf()
{
	const Vector3R albedo = packet.getVector3r("albedo", Vector3R(0.5_r), DataTreatment::OPTIONAL("all components are set to 0.5"));
	setAlbedo(albedo);
}

MatteOpaque::~MatteOpaque() = default;

void MatteOpaque::populateSurfaceBehavior(SurfaceBehavior* const out_surfaceBehavior) const
{
	out_surfaceBehavior->setBsdf(std::make_unique<LambertianDiffuse>(m_bsdf));
}

void MatteOpaque::setAlbedo(const Vector3R& albedo)
{
	setAlbedo(albedo.x, albedo.y, albedo.z);
}

void MatteOpaque::setAlbedo(const real r, const real g, const real b)
{
	setAlbedo(std::make_shared<ConstantTexture>(r, g, b));
}

void MatteOpaque::setAlbedo(const std::shared_ptr<Texture>& albedo)
{
	m_bsdf.setAlbedo(albedo);
}

}// end namespace ph