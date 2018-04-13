#include "Core/Emitter/SurfaceEmitter.h"
#include "Common/assertion.h"
#include "Core/Texture/TConstantTexture.h"
#include "Core/Quantity/ColorSpace.h"

namespace ph
{

SurfaceEmitter::SurfaceEmitter() : 
	Emitter(),
	m_emittedRadiance(nullptr)
{
	SpectralStrength defaultRadiance;
	defaultRadiance.setSampled(ColorSpace::get_D65_SPD());
	setEmittedRadiance(std::make_shared<TConstantTexture<SpectralStrength>>(defaultRadiance));
}

SurfaceEmitter::SurfaceEmitter(const std::shared_ptr<TTexture<SpectralStrength>>& emittedRadiance) : 
	Emitter(),
	m_emittedRadiance(nullptr)
{
	setEmittedRadiance(emittedRadiance);
}

SurfaceEmitter::~SurfaceEmitter() = default;

void SurfaceEmitter::setEmittedRadiance(
	const std::shared_ptr<TTexture<SpectralStrength>>& emittedRadiance)
{
	PH_ASSERT(emittedRadiance != nullptr);

	m_emittedRadiance = emittedRadiance;
}

}// end namespace ph