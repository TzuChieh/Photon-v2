#include "Engine/World/SceneGlobals.h"
#include "Engine/Math/Color/Spectrum.h"
#include "Engine/Core/Texture/constant_textures.h"
#include "Engine/Core/SurfaceBehavior/Property/ExactDielectricFresnel.h"
#include "Engine/Core/SurfaceBehavior/Property/surface_property.h"
#include "Engine/Core/SurfaceBehavior/SurfaceOptics/TIdealDielectricTransmitter.h"
#include "Engine/Core/VolumeBehavior/Property/ExponentialTransmittance.h"
#include "Engine/Core/Emitter/ZeroSurfaceEmitter.h"
#include "Engine/Core/Intersection/PrimitiveMetadata.h"

namespace ph
{

std::shared_ptr<TTexture<math::Spectrum>> SceneGlobals::getZeroTexture()
{
	static auto texture = std::make_shared<TConstantTexture<math::Spectrum>>(math::Spectrum(0));
	return texture;
}

std::shared_ptr<DielectricFresnel> SceneGlobals::getFullyTransmissiveFresnel()
{
	// Note that Schlick's approximation is NOT exact for the fully transmissive case
	static auto fresnel = std::make_shared<ExactDielectricFresnel>(1.0_r, 1.0_r);
	return fresnel;
}

std::shared_ptr<TransmittanceFunction> SceneGlobals::getExponentialTransmittance()
{
	static auto transmittance = std::make_shared<ExponentialTransmittance>();
	return transmittance;
}

const SurfaceOptics* SceneGlobals::getFullyTransmissiveSurfaceOptics()
{
	using Scale = TConstantSurfaceProperty<math::Spectrum>;

	static TIdealDielectricTransmitter<Scale> optics{getFullyTransmissiveFresnel()};
	return &optics;
}

const SurfaceEmitter* SceneGlobals::getZeroSurfaceEmitter()
{
	static ZeroSurfaceEmitter emitter{};
	return &emitter;
}

const PrimitiveMetadata* SceneGlobals::getDefaultPrimitiveMetadata()
{
	static PrimitiveMetadata metadata = 
		[]()
		{
			PrimitiveMetadata metadata;
			metadata.surface().setOptics(nullptr);
			metadata.surface().setEmitter(nullptr);
			return metadata;
		}();

	return &metadata;
}

}// end namespace ph
