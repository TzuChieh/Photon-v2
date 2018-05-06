#pragma once

#include "Actor/Material/SurfaceMaterial.h"
#include "Core/SurfaceBehavior/SurfaceOptics/TranslucentMicrofacet.h"
#include "Common/primitive_type.h"
#include "Math/TVector3.h"
#include "FileIO/SDL/TCommandInterface.h"

namespace ph
{

class AbradedTranslucent : public SurfaceMaterial, public TCommandInterface<AbradedTranslucent>
{
public:
	AbradedTranslucent();
	virtual ~AbradedTranslucent() override;

	//void setAlbedo(const Vector3R& albedo);
	void setIor(const real iorOuter, const real iorInner);
	void setRoughness(const real roughness);

private:
	virtual std::shared_ptr<SurfaceOptics> genSurfaceOptics(CookingContext& context) const override;

	TranslucentMicrofacet m_optics;

// command interface
public:
	AbradedTranslucent(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
};

}// end namespace ph