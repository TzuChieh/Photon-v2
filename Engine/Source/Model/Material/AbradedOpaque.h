#pragma once

#include "Model/Material/Material.h"
#include "Model/Material/OpaqueMicrofacetSurfaceIntegrand.h"
#include "Common/primitive_type.h"
#include "Math/Vector3f.h"

namespace ph
{

class AbradedOpaque : public Material
{
public:
	AbradedOpaque();
	virtual ~AbradedOpaque() override;

	virtual inline const SurfaceIntegrand* getSurfaceIntegrand() const override
	{
		return &m_surfaceIntegrand;
	}

	inline float32 getRoughness() const
	{
		return m_roughness;
	}

	inline void setRoughness(const float32 roughness)
	{
		m_roughness = roughness;
	}

	inline const Vector3f& getF0() const
	{
		return m_f0;
	}

	inline void setF0(const Vector3f& f0)
	{
		m_f0.set(f0);
	}

private:
	OpaqueMicrofacetSurfaceIntegrand m_surfaceIntegrand;

	float32 m_roughness;
	Vector3f m_f0;
};

}// end namespace ph