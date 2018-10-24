#pragma once

#include "Actor/Material/SurfaceMaterial.h"
#include "Core/SurfaceBehavior/SurfaceOptics/TranslucentMicrofacet.h"
#include "Common/primitive_type.h"
#include "Math/TVector3.h"
#include "FileIO/SDL/TCommandInterface.h"

#include <functional>

namespace ph
{

class AbradedTranslucent : public SurfaceMaterial, public TCommandInterface<AbradedTranslucent>
{
public:
	AbradedTranslucent();
	~AbradedTranslucent() override;

	void genSurface(CookingContext& context, SurfaceBehavior& behavior) const override;

	//void setAlbedo(const Vector3R& albedo);

private:
	std::function<std::unique_ptr<SurfaceOptics>()> m_opticsGenerator;

// command interface
public:
	explicit AbradedTranslucent(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
};

}// end namespace ph