#pragma once

#include "Actor/Material/Material.h"
#include "Core/SurfaceBehavior/LambertianDiffuse.h"
#include "Math/TVector3.h"
#include "Actor/Texture/Texture.h"

#include <memory>

namespace ph
{

class MatteOpaque : public Material
{
public:
	MatteOpaque();
	MatteOpaque(const InputPacket& packet);
	virtual ~MatteOpaque() override;

	virtual void populateSurfaceBehavior(SurfaceBehavior* const out_surfaceBehavior) const override;

	void setAlbedo(const Vector3R& albedo);
	void setAlbedo(const real r, const real g, const real b);
	void setAlbedo(const std::shared_ptr<Texture>& albedo);

private:
	LambertianDiffuse m_bsdfCos;
};

}// end namespace ph