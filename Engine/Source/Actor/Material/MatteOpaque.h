#pragma once

#include "Actor/Material/SurfaceMaterial.h"
#include "Math/math_fwd.h"
#include "FileIO/SDL/TCommandInterface.h"
#include "Actor/Image/Image.h"

#include <memory>

namespace ph
{

class MatteOpaque : public SurfaceMaterial, public TCommandInterface<MatteOpaque>
{
public:
	MatteOpaque();
	MatteOpaque(const Vector3R& linearSrgbAlbedo);
	virtual ~MatteOpaque() override;

	void setAlbedo(const Vector3R& albedo);
	void setAlbedo(const real r, const real g, const real b);
	void setAlbedo(const std::shared_ptr<Image>& albedo);

private:
	virtual std::shared_ptr<SurfaceOptics> genSurfaceOptics(CookingContext& context) const override;

private:
	std::shared_ptr<Image> m_albedo;

// command interface
public:
	MatteOpaque(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
};

}// end namespace ph