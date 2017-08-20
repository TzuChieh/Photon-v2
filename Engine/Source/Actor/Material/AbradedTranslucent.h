#pragma once

#include "Actor/Material/Material.h"
#include "Core/SurfaceBehavior/BSDF/TranslucentMicrofacet.h"
#include "Common/primitive_type.h"
#include "Math/TVector3.h"
#include "FileIO/SDL/TCommandInterface.h"

namespace ph
{

class AbradedTranslucent : public Material, public TCommandInterface<AbradedTranslucent>
{
public:
	AbradedTranslucent();
	virtual ~AbradedTranslucent() override;

	virtual void populateSurfaceBehavior(SurfaceBehavior* const out_surfaceBehavior) const override;

	//void setAlbedo(const Vector3R& albedo);
	void setIor(const real iorOuter, const real iorInner);
	void setRoughness(const real roughness);

private:
	TranslucentMicrofacet m_bsdf;

	static real roughnessToAlpha(const real roughness);

// command interface
public:
	static SdlTypeInfo ciTypeInfo();
	static std::unique_ptr<AbradedTranslucent> ciLoad(const InputPacket& packet);
	static ExitStatus ciExecute(const std::shared_ptr<AbradedTranslucent>& targetResource, const std::string& functionName, const InputPacket& packet);
};

}// end namespace ph