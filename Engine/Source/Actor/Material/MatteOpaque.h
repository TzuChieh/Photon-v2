#pragma once

#include "Actor/Material/Material.h"
#include "Core/SurfaceBehavior/SurfaceOptics/LambertianDiffuse.h"
#include "Math/TVector3.h"
#include "Core/Texture/Texture.h"
#include "FileIO/SDL/TCommandInterface.h"

#include <memory>

namespace ph
{

class MatteOpaque : public Material, public TCommandInterface<MatteOpaque>
{
public:
	MatteOpaque();
	virtual ~MatteOpaque() override;

	virtual void populateSurfaceBehavior(SurfaceBehavior* out_surfaceBehavior) const override;

	void setAlbedo(const Vector3R& albedo);
	void setAlbedo(const real r, const real g, const real b);
	void setAlbedo(const std::shared_ptr<Texture>& albedo);

private:
	LambertianDiffuse m_optics;

// command interface
public:
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
	static std::unique_ptr<MatteOpaque> ciLoad(const InputPacket& packet);
};

}// end namespace ph