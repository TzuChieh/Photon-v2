#include "Actor/Material/AbradedTranslucent.h"
#include "FileIO/InputPacket.h"
#include "Core/SurfaceBehavior/Property/SchlickApproxDielectricFresnel.h"
#include "Core/SurfaceBehavior/Property/ExactDielectricFresnel.h"
#include "Core/SurfaceBehavior/Property/IsoTrowbridgeReitz.h"
#include "Actor/Material/Utility/RoughnessToAlphaMapping.h"

#include <memory>
#include <cmath>
#include <algorithm>

namespace ph
{

AbradedTranslucent::AbradedTranslucent() :
	Material(),
	m_optics()
{
	
}

AbradedTranslucent::~AbradedTranslucent() = default;

void AbradedTranslucent::populateSurfaceBehavior(CookingContext& context, SurfaceBehavior* const out_surfaceBehavior) const
{
	out_surfaceBehavior->setSurfaceOptics(std::make_unique<TranslucentMicrofacet>(m_optics));
}

//void AbradedTranslucent::setAlbedo(const Vector3R& albedo)
//{
//	m_bsdf.setF0(std::make_shared<ConstantTexture>(albedo));
//}

//void AbradedTranslucent::setF0(const real iorOuter, const real iorInner)
//{
//	setF0(f0.x, f0.y, f0.z);
//}

//void AbradedTranslucent::setF0(const real r, const real g, const real b)
//{
//	m_bsdf.setF0(std::make_shared<ConstantTexture>(r, g, b));
//}

void AbradedTranslucent::setIor(const real iorOuter, const real iorInner)
{
	//m_bsdf.setFrenelEffect(std::make_shared<SchlickApproxDielectricFresnel>(iorOuter, iorInner));
	m_optics.setFrenelEffect(std::make_shared<ExactDielectricFresnel>(iorOuter, iorInner));
}

void AbradedTranslucent::setRoughness(const real roughness)
{
	const real alpha = RoughnessToAlphaMapping::pbrtV3(roughness);
	m_optics.setMicrofacet(std::make_shared<IsoTrowbridgeReitz>(alpha));
}

// command interface

SdlTypeInfo AbradedTranslucent::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_MATERIAL, "abraded-translucent");
}

void AbradedTranslucent::ciRegister(CommandRegister& cmdRegister)
{
	SdlLoader loader;
	loader.setFunc<AbradedTranslucent>(ciLoad);
	cmdRegister.setLoader(loader);
}

std::unique_ptr<AbradedTranslucent> AbradedTranslucent::ciLoad(const InputPacket& packet)
{
	Vector3R albedo(0.5f, 0.5f, 0.5f);
	Vector3R f0(0.04f, 0.04f, 0.04f);
	real roughness = 0.5f;
	real ior       = 1.0f;

	albedo    = packet.getVector3r("albedo", albedo);
	f0        = packet.getVector3r("f0", f0);
	roughness = packet.getReal("roughness", roughness);
	ior       = packet.getReal("ior", ior);

	std::unique_ptr<AbradedTranslucent> material = std::make_unique<AbradedTranslucent>();
	//material->setAlbedo(albedo);
	//material->setF0(f0);
	material->setRoughness(roughness);
	material->setIor(1, ior);
	return material;
}

}// end namespace ph