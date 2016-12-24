#pragma once

#include "Entity/Material/Material.h"
#include "Entity/Material/SurfaceBehavior/STranslucentMicrofacet.h"
#include "Common/primitive_type.h"
#include "Math/Vector3f.h"

namespace ph
{

class AbradedTranslucent : public Material
{
public:
	AbradedTranslucent();
	virtual ~AbradedTranslucent() override;

	virtual inline const SurfaceBehavior* getSurfaceBehavior() const override
	{
		return &m_surfaceBehavior;
	}

	void setF0(const Vector3f& f0);
	void setF0(const float32 r, const float32 g, const float32 b);
	void setIOR(const float32 ior);
	void setRoughness(const float32 roughness);

private:
	STranslucentMicrofacet m_surfaceBehavior;
};

}// end namespace ph