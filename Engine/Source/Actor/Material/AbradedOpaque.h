#pragma once

#include "Actor/Material/Material.h"
#include "Core/SurfaceBehavior/OpaqueMicrofacet.h"
#include "Common/primitive_type.h"
#include "Math/Vector3f.h"

namespace ph
{

class AbradedOpaque : public Material
{
public:
	AbradedOpaque();
	virtual ~AbradedOpaque() override;

	virtual void populateSurfaceBehavior(SurfaceBehavior* const out_surfaceBehavior) const override;

	void setRoughness(const float32 roughness);
	void setF0(const Vector3f& f0);
	void setF0(const float32 r, const float32 g, const float32 b);

private:
	OpaqueMicrofacet m_bsdfCos;
};

}// end namespace ph