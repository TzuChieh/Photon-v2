#pragma once

#include "Actor/Material/Material.h"
#include "Core/SurfaceBehavior/BSDF/OpaqueMicrofacet.h"
#include "Common/primitive_type.h"
#include "Math/TVector3.h"
#include "FileIO/SDL/TCommandInterface.h"

namespace ph
{

class AbradedOpaque : public Material, public TCommandInterface<AbradedOpaque>
{
public:
	AbradedOpaque();
	virtual ~AbradedOpaque() override;

	virtual void populateSurfaceBehavior(SurfaceBehavior* const out_surfaceBehavior) const override;

	void setAlbedo(const Vector3R& albedo);
	void setRoughness(const real roughness);
	void setF0(const Vector3R& f0);
	void setF0(const real r, const real g, const real b);

private:
	OpaqueMicrofacet m_bsdf;

	static real roughnessToAlpha(const real roughness);

// command interface
public:
	AbradedOpaque(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static ExitStatus ciExecute(const std::shared_ptr<AbradedOpaque>& targetResource, const std::string& functionName, const InputPacket& packet);
};

}// end namespace ph