#pragma once

#include "Actor/Material/Material.h"
#include "Core/SurfaceBehavior/OpaqueMicrofacet.h"
#include "Common/primitive_type.h"
#include "Math/TVector3.h"

namespace ph
{

class AbradedOpaque : public Material
{
public:
	AbradedOpaque();
	AbradedOpaque(const InputPacket& packet);
	virtual ~AbradedOpaque() override;

	virtual void populateSurfaceBehavior(SurfaceBehavior* const out_surfaceBehavior) const override;

	void setAlbedo(const Vector3R& albedo);
	void setRoughness(const real roughness);
	void setF0(const Vector3R& f0);
	void setF0(const real r, const real g, const real b);

private:
	OpaqueMicrofacet m_bsdfCos;

	static float32 roughnessToAlpha(const real roughness);
};

}// end namespace ph