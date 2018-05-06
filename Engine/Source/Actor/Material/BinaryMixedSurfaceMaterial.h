#pragma once

#include "Actor/Material/SurfaceMaterial.h"
#include "Actor/Image/Image.h"
#include "Common/primitive_type.h"

#include <memory>

namespace ph
{

class BinaryMixedSurfaceMaterial final : 

	public SurfaceMaterial, 
	public TCommandInterface<BinaryMixedSurfaceMaterial>
{
public:
	enum class EMode
	{
		LERP
	};

	BinaryMixedSurfaceMaterial();
	virtual ~BinaryMixedSurfaceMaterial() override;

	void setMode(EMode mode);
	void setMaterials(
		const std::shared_ptr<SurfaceMaterial>& material0,
		const std::shared_ptr<SurfaceMaterial>& material1);
	void setFactor(real factor);
	void setFactor(const std::shared_ptr<Image>& factor);

private:
	virtual std::shared_ptr<SurfaceOptics> genSurfaceOptics(CookingContext& context) const override;

	EMode                            m_mode;
	std::shared_ptr<SurfaceMaterial> m_material0;
	std::shared_ptr<SurfaceMaterial> m_material1;
	std::shared_ptr<Image>           m_factor;

// command interface
public:
	BinaryMixedSurfaceMaterial(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
};

}// end namespace ph