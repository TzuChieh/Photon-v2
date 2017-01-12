#pragma once

#include "Actor/Material/Material.h"
#include "Core/SurfaceBehavior/TranslucentMicrofacet.h"
#include "Common/primitive_type.h"
#include "Math/Vector3f.h"

namespace ph
{

class AbradedTranslucent : public Material
{
public:
	AbradedTranslucent();
	virtual ~AbradedTranslucent() override;

	virtual void populateSurfaceBehavior(SurfaceBehavior* const out_surfaceBehavior) const override;

	void setF0(const Vector3f& f0);
	void setF0(const float32 r, const float32 g, const float32 b);
	void setIOR(const float32 ior);
	void setRoughness(const float32 roughness);

private:
	TranslucentMicrofacet m_bsdfCos;

	static float32 roughnessToAlpha(const float32 roughness);
};

}// end namespace ph