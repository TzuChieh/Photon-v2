#include "Actor/Material/AbradedTranslucent.h"
#include "FileIO/SDL/InputPacket.h"
#include "Core/SurfaceBehavior/Property/DielectricFresnel.h"
#include "Core/SurfaceBehavior/Property/SchlickApproxDielectricFresnel.h"
#include "Core/SurfaceBehavior/Property/ExactDielectricFresnel.h"
#include "Core/SurfaceBehavior/Property/IsoTrowbridgeReitz.h"
#include "Actor/Material/Utility/RoughnessToAlphaMapping.h"
#include "Common/assertion.h"
#include "Core/SurfaceBehavior/SurfaceBehavior.h"

#include <memory>
#include <cmath>
#include <algorithm>

namespace ph
{

AbradedTranslucent::AbradedTranslucent() :
	SurfaceMaterial(),

	m_opticsGenerator(),
	m_interfaceInfo  ()
{}

void AbradedTranslucent::genSurface(CookingContext& context, SurfaceBehavior& behavior) const
{
	PH_ASSERT(m_opticsGenerator);

	behavior.setOptics(m_opticsGenerator());
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

// command interface

AbradedTranslucent::AbradedTranslucent(const InputPacket& packet) : 
	SurfaceMaterial(packet),

	m_opticsGenerator(),
	m_interfaceInfo  (packet)
{
	const Vector3R albedo    = packet.getVector3("albedo", Vector3R(0.5_r, 0.5_r, 0.5_r));
	const real     roughness = packet.getReal("roughness", 0.5_r);

	std::shared_ptr<DielectricFresnel> fresnelEffect = DielectricInterfaceInfo(packet).genFresnelEffect();
	PH_ASSERT(fresnelEffect);

	//material->setAlbedo(albedo);
	//material->setF0(f0);

	m_opticsGenerator = [=]()
	{
		const real alpha = RoughnessToAlphaMapping::squared(roughness);

		auto optics = std::make_unique<TranslucentMicrofacet>(
			fresnelEffect,
			std::make_shared<IsoTrowbridgeReitz>(alpha));

		return optics;
	};
}

SdlTypeInfo AbradedTranslucent::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_MATERIAL, "abraded-translucent");
}

void AbradedTranslucent::ciRegister(CommandRegister& cmdRegister)
{
	SdlLoader loader;
	loader.setFunc<AbradedTranslucent>([](const InputPacket& packet)
	{
		return std::make_unique<AbradedTranslucent>(packet);
	});
	cmdRegister.setLoader(loader);
}

}// end namespace ph
