#include "Actor/Material/BinaryMixedSurfaceMaterial.h"
#include "FileIO/SDL/InputPacket.h"
#include "Common/assertion.h"
#include "Actor/Image/ConstantImage.h"
#include "Core/SurfaceBehavior/SurfaceOptics/LerpedSurfaceOptics.h"

#include <iostream>

namespace ph
{

BinaryMixedSurfaceMaterial::BinaryMixedSurfaceMaterial() : 
	SurfaceMaterial(),
	m_mode(EMode::LERP),
	m_material0(nullptr), m_material1(nullptr),
	m_factor(nullptr)
{}

BinaryMixedSurfaceMaterial::~BinaryMixedSurfaceMaterial() = default;

std::shared_ptr<SurfaceOptics> BinaryMixedSurfaceMaterial::genSurfaceOptics(CookingContext& context) const
{
	if(m_material0 == nullptr || m_material1 == nullptr)
	{
		// TODO: logger
		std::cerr << "warning: at BinaryMixedSurfaceMaterial(), " 
		          << "material is empty" << std::endl;
		return nullptr;
	}

	auto optics0 = m_material0->genSurfaceOptics(context);
	auto optics1 = m_material1->genSurfaceOptics(context);

	switch(m_mode)
	{
	case EMode::LERP:
		if(m_factor != nullptr)
		{
			auto factor = m_factor->genTextureSpectral(context);
			return std::make_shared<LerpedSurfaceOptics>(optics0, optics1, factor);
		}
		else
		{
			return std::make_shared<LerpedSurfaceOptics>(optics0, optics1);
		}
	default:
		// TODO: logger
		std::cerr << "warning: at BinaryMixedSurfaceMaterial(), " 
		          << "unsupported material mixing mode" << std::endl;
		return nullptr;
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
	PH_ASSERT(material0 != nullptr && material1 != nullptr);

	m_material0 = material0;
	m_material1 = material1;
}

void BinaryMixedSurfaceMaterial::setFactor(const real factor)
{
	setFactor(std::make_shared<ConstantImage>(factor, ConstantImage::EType::ECF_LINEAR_SRGB));
}

void BinaryMixedSurfaceMaterial::setFactor(const std::shared_ptr<Image>& factor)
{
	m_factor = factor;
}

// command interface

BinaryMixedSurfaceMaterial::BinaryMixedSurfaceMaterial(const InputPacket& packet) : 
	SurfaceMaterial(packet),
	m_mode(EMode::LERP),
	m_material0(nullptr), m_material1(nullptr),
	m_factor(nullptr)
{
	// TODO: support factor other than real

	std::string mode      = packet.getString("mode");
	auto        material0 = packet.get<SurfaceMaterial>("material-0", DataTreatment::REQUIRED());
	auto        material1 = packet.get<SurfaceMaterial>("material-1", DataTreatment::REQUIRED());
	real        factor    = packet.getReal("factor", 0.5_r);

	setMaterials(material0, material1);
	setFactor(factor);
	if(mode == "lerp")
	{
		setMode(EMode::LERP);
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