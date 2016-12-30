#pragma once

#include "Core/SurfaceBehavior/BSDFcos.h"
#include "Image/Texture.h"

#include <memory>

namespace ph
{

class TranslucentMicrofacet : public BSDFcos
{
public:
	TranslucentMicrofacet();
	virtual ~TranslucentMicrofacet() override;

	virtual void genImportanceSample(const Intersection& intersection, const Ray& ray, SurfaceSample* const out_sample) const override;
	virtual void evaluate(const Intersection& intersection, const Vector3f& wi, const Vector3f& wo, Vector3f* const out_value) const override;

	inline void setF0(const std::shared_ptr<Texture>& f0)
	{
		m_F0 = f0;
	}

	inline void setIOR(const std::shared_ptr<Texture>& ior)
	{
		m_IOR = ior;
	}

	inline void setRoughness(const std::shared_ptr<Texture>& roughness)
	{
		m_roughness = roughness;
	}

private:
	std::shared_ptr<Texture> m_F0;
	std::shared_ptr<Texture> m_IOR;
	std::shared_ptr<Texture> m_roughness;
};

}// end namespace ph