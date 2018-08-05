#include "Core/Emitter/SurfaceEmitter.h"
#include "Common/assertion.h"
#include "Core/Texture/TConstantTexture.h"
#include "Core/Quantity/ColorSpace.h"
#include "Math/Math.h"

namespace ph
{

SurfaceEmitter::SurfaceEmitter() : 
	Emitter(),
	m_emittedRadiance(nullptr),
	m_isBackFaceEmission(false)
{
	SpectralStrength defaultRadiance;
	defaultRadiance.setSampled(ColorSpace::get_D65_SPD());
	setEmittedRadiance(std::make_shared<TConstantTexture<SpectralStrength>>(defaultRadiance));
}

SurfaceEmitter::SurfaceEmitter(const std::shared_ptr<TTexture<SpectralStrength>>& emittedRadiance) : 
	Emitter(),
	m_emittedRadiance(nullptr),
	m_isBackFaceEmission(false)
{
	setEmittedRadiance(emittedRadiance);
}

void SurfaceEmitter::setEmittedRadiance(
	const std::shared_ptr<TTexture<SpectralStrength>>& emittedRadiance)
{
	PH_ASSERT(emittedRadiance != nullptr);

	m_emittedRadiance = emittedRadiance;
}

bool SurfaceEmitter::canEmit(const Vector3R& emitDirection, const Vector3R& N) const
{
	return Math::isSameHemisphere(emitDirection, N) != m_isBackFaceEmission;
}

void SurfaceEmitter::setFrontFaceEmit()
{
	m_isBackFaceEmission = false;
}

void SurfaceEmitter::setBackFaceEmit()
{
	m_isBackFaceEmission = true;
}

}// end namespace ph