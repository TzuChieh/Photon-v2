#pragma once

#include "Engine/Math/Color/spectrum_fwd.h"
#include "Engine/Core/Texture/texture_fwd.h"

#include <memory>

namespace ph
{

class DielectricFresnel;
class TransmittanceFunction;
class SurfaceOptics;
class SurfaceEmitter;
class PrimitiveMetadata;

class SceneGlobals final
{
public:
	static std::shared_ptr<TTexture<math::Spectrum>> getZeroTexture();
	static std::shared_ptr<DielectricFresnel> getFullyTransmissiveFresnel();
	static std::shared_ptr<TransmittanceFunction> getExponentialTransmittance();

	static const SurfaceOptics* getFullyTransmissiveSurfaceOptics();
	static const SurfaceEmitter* getZeroSurfaceEmitter();
	static const PrimitiveMetadata* getDefaultPrimitiveMetadata();
};

}// end namespace ph
