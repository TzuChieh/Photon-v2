#include "Actor/Material/BinaryMixedSurfaceMaterial.h"
#include "DataIO/SDL/InputPacket.h"
#include "Common/assertion.h"
#include "Actor/Image/ConstantImage.h"
#include "Core/SurfaceBehavior/SurfaceOptics/LerpedSurfaceOptics.h"
#include "Core/SurfaceBehavior/SurfaceBehavior.h"

#include <iostream>

namespace ph
{

BinaryMixedSurfaceMaterial::BinaryMixedSurfaceMaterial() : 
	SurfaceMaterial(),
	m_mode(EMode::LERP),
	m_material0(nullptr), m_material1(nullptr),
	m_factor(nullptr)
{}

void BinaryMixedSurfaceMaterial::genSurface(CookingContext& context, SurfaceBehavior& behavior) const
{
	if(!m_material0 || !m_material1)
	{
		// TODO: logger
		std::cerr << "warning: at BinaryMixedSurfaceMaterial(), " 
		          << "material is empty" << std::endl;
		return;
	}

	SurfaceBehavior behavior0, behavior1;
	m_material0->genSurface(context, behavior0);
	m_material1->genSurface(context, behavior1);
	auto optics0 = behavior0.getOpticsResource();
	auto optics1 = behavior1.getOpticsResource();
	if(!optics0 || !optics1)
	{
		// TODO: logger
		std::cerr << "warning: at BinaryMixedSurfaceMaterial(), "
		          << "optics is empty" << std::endl;
		return;
	}

	switch(m_mode)
	{
	case EMode::LERP:
		if(m_factor != nullptr)
		{
			auto factor = m_factor->genTextureSpectral(context);
			behavior.setOptics(std::make_shared<LerpedSurfaceOptics>(optics0, optics1, factor));
		}
		else
		{
			behavior.setOptics(std::make_shared<LerpedSurfaceOptics>(optics0, optics1));
		}
		break;

	default:
		// TODO: logger
		std::cerr << "warning: at BinaryMixedSurfaceMaterial(), " 
		          << "unsupported material mixing mode" << std::endl;
		break;
	}
}

void BinaryMixedSurfaceMaterial::setMode(const EMode mode)
{
	m_mode = mode;
}

void BinaryMixedSurfaceMaterial::setMaterials(
	const std::shared_ptr<SurfaceMaterial>& material0,
	const std::shared_ptr<SurfaceMaterial>& material1)
{
	PH_ASSERT(material0);
	PH_ASSERT(material1);

	m_material0 = material0;
	m_material1 = material1;
}

void BinaryMixedSurfaceMaterial::setFactor(const real factor)
{
	setFactor(std::make_shared<ConstantImage>(factor, ConstantImage::EType::ECF_LINEAR_SRGB));
}

void BinaryMixedSurfaceMaterial::setFactor(const std::shared_ptr<Image>& factor)
{
	PH_ASSERT(factor);

	m_factor = factor;
}

// command interface

BinaryMixedSurfaceMaterial::BinaryMixedSurfaceMaterial(const InputPacket& packet) : 

	SurfaceMaterial(packet),

	m_mode     (EMode::LERP),
	m_material0(nullptr), 
	m_material1(nullptr),
	m_factor   (nullptr)
{
	const auto mode = packet.getString("mode");
	if(mode == "lerp")
	{
		setMode(EMode::LERP);
	}

	const auto material0 = packet.getReference<SurfaceMaterial>("material-0", DataTreatment::REQUIRED());
	const auto material1 = packet.getReference<SurfaceMaterial>("material-1", DataTreatment::REQUIRED());
	setMaterials(material0, material1);

	// TODO: vector3 factor
	if(packet.hasReference<Image>("factor"))
	{
		const auto factor = packet.getReference<Image>("factor");
		setFactor(factor);
	}
	else
	{
		const auto factor = packet.getReal("factor", 0.5_r);
		setFactor(factor);
	}
}

SdlTypeInfo BinaryMixedSurfaceMaterial::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_MATERIAL, "binary-mixed-surface");
}

void BinaryMixedSurfaceMaterial::ciRegister(CommandRegister& cmdRegister)
{
	SdlLoader loader;
	loader.setFunc<BinaryMixedSurfaceMaterial>([](const InputPacket& packet)
	{
		return std::make_unique<BinaryMixedSurfaceMaterial>(packet);
	});
	cmdRegister.setLoader(loader);
}

}// end namespace ph
