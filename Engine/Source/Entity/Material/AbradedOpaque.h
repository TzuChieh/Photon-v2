#pragma once

#include "Entity/Material/Material.h"
#include "Entity/Material/SurfaceBehavior/SOpaqueMicrofacet.h"
#include "Common/primitive_type.h"
#include "Math/Vector3f.h"

namespace ph
{

class AbradedOpaque : public Material
{
public:
	AbradedOpaque();
	virtual ~AbradedOpaque() override;

	virtual inline const SurfaceBehavior* getSurfaceBehavior() const override
	{
		return &m_surfaceBehavior;
	}

	void setRoughness(const float32 roughness);
	void setF0(const Vector3f& f0);
	void setF0(const float32 r, const float32 g, const float32 b);

private:
	SOpaqueMicrofacet m_surfaceBehavior;
};

}// end namespace ph