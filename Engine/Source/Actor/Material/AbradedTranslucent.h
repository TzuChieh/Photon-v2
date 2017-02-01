#pragma once

#include "Actor/Material/Material.h"
#include "Core/SurfaceBehavior/TranslucentMicrofacet.h"
#include "Common/primitive_type.h"
#include "Math/TVector3.h"

namespace ph
{

class AbradedTranslucent : public Material
{
public:
	AbradedTranslucent();
	AbradedTranslucent(const InputPacket& packet);
	virtual ~AbradedTranslucent() override;

	virtual void populateSurfaceBehavior(SurfaceBehavior* const out_surfaceBehavior) const override;

	void setAlbedo(const Vector3R& albedo);
	void setF0(const Vector3R& f0);
	void setF0(const real r, const real g, const real b);
	void setIOR(const real ior);
	void setRoughness(const real roughness);

private:
	TranslucentMicrofacet m_bsdfCos;

	static real roughnessToAlpha(const real roughness);
};

}// end namespace ph