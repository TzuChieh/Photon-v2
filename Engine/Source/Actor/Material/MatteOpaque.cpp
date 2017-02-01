#include "Actor/Material/MatteOpaque.h"
#include "Actor/Texture/ConstantTexture.h"
#include "FileIO/InputPacket.h"

namespace ph
{

MatteOpaque::MatteOpaque() : 
	Material(), 
	m_bsdfCos()
{
	
}

MatteOpaque::MatteOpaque(const InputPacket& packet) :
	Material(packet), 
	m_bsdfCos()
{
	const Vector3R albedo = packet.getVector3R("albedo", Vector3R(0.5f, 0.5f, 0.5f),
	                                           "albedo not found, all components are set to 0.5");
	setAlbedo(albedo);
}

MatteOpaque::~MatteOpaque() = default;

void MatteOpaque::populateSurfaceBehavior(SurfaceBehavior* const out_surfaceBehavior) const
{
	out_surfaceBehavior->setBsdfCos(std::make_unique<LambertianDiffuse>(m_bsdfCos));
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
	m_bsdfCos.setAlbedo(albedo);
}

}// end namespace ph