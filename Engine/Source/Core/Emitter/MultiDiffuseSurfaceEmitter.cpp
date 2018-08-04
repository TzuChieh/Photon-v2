#include "Core/Emitter/MultiDiffuseSurfaceEmitter.h"
#include "Common/assertion.h"
#include "Core/Intersectable/Primitive.h"
#include "Core/Texture/TSampler.h"
#include "Core/Quantity/SpectralStrength.h"
#include "Core/Texture/TTexture.h"
#include "Common/utility.h"
#include "Math/Random.h"
#include "Core/Sample/PositionSample.h"
#include "Core/Sample/DirectLightSample.h"

#include <iostream>

namespace ph
{

MultiDiffuseSurfaceEmitter::MultiDiffuseSurfaceEmitter(const std::vector<DiffuseSurfaceEmitter>& emitters) :
	SurfaceEmitter(),
	m_emitters()
{
	PH_ASSERT(!emitters.empty());

	m_extendedArea = 0.0_r;
	for(const auto& emitter : emitters)
	{
		const Primitive* primitive = emitter.getSurface();

		PH_ASSERT(primitive != nullptr);
		m_extendedArea += primitive->calcExtendedArea();

		addEmitter(emitter);
	}
	m_reciExtendedArea = 1.0_r / m_extendedArea;
}

MultiDiffuseSurfaceEmitter::~MultiDiffuseSurfaceEmitter() = default;

//static int iii = 0;
void MultiDiffuseSurfaceEmitter::evalEmittedRadiance(const SurfaceHit& X, SpectralStrength* out_radiance) const
{
	// FIXME: sort of hacked... (the direction of ray is reversed)
	// only front side of the emitter is emissive
	if(!canEmit(X.getIncidentRay().getDirection().mul(-1.0_r), X.getShadingNormal()))
	{
		/*std::cerr << X.getIncidentRay().getDirection().mul(-1.0_r).dot(X.getShadingNormal()) << std::endl;
		std::cerr << ++iii << std::endl;*/

		out_radiance->setValues(0.0_r);
		return;
	}

	// TODO: able to specify channel or restrict it
	TSampler<SpectralStrength> sampler(EQuantity::EMR);
	*out_radiance = sampler.sample(getEmittedRadiance(), X);
}

void MultiDiffuseSurfaceEmitter::genDirectSample(DirectLightSample& sample) const
{
	const DiffuseSurfaceEmitter& emitter = m_emitters[Random::genUniformIndex_iL_eU(0, m_emitters.size())];

	emitter.genDirectSample(sample);
	const real pickPdf = (1.0_r / static_cast<real>(m_emitters.size()));
	sample.pdfW *= pickPdf;
}

void MultiDiffuseSurfaceEmitter::genSensingRay(Ray* out_ray, SpectralStrength* out_Le, Vector3R* out_eN, real* out_pdfA, real* out_pdfW) const
{
	// TODO
	PH_ASSERT_UNREACHABLE_SECTION();
}

real MultiDiffuseSurfaceEmitter::calcDirectSamplePdfW(const Vector3R& targetPos, const Vector3R& emitPos, const Vector3R& emitN, const Primitive* hitPrim) const
{
	// HACK

	const Vector3R emitDir(targetPos.sub(emitPos).normalizeLocal());
	if(!canEmit(emitDir, emitN))
	{
		return 0.0_r;
	}
	
	const real emitDirDotNormal = emitDir.dot(emitN);
	const real pickPdf = (1.0_r / static_cast<real>(m_emitters.size()));
	const real samplePdfA  = hitPrim->calcPositionSamplePdfA(emitPos);
	const real distSquared = targetPos.sub(emitPos).lengthSquared();
	return samplePdfA / std::abs(emitDirDotNormal) * distSquared * pickPdf;
}

void MultiDiffuseSurfaceEmitter::setEmittedRadiance(const std::shared_ptr<TTexture<SpectralStrength>>& emittedRadiance)
{
	SurfaceEmitter::setEmittedRadiance(emittedRadiance);

	for(auto& emitter : m_emitters)
	{
		emitter.setEmittedRadiance(emittedRadiance);
	}
}

void MultiDiffuseSurfaceEmitter::addEmitter(const DiffuseSurfaceEmitter& emitter)
{
	m_emitters.push_back(emitter);

	if(m_isBackFaceEmission)
	{
		m_emitters.back().setBackFaceEmit();
	}
	else
	{
		m_emitters.back().setFrontFaceEmit();
	}
}

void MultiDiffuseSurfaceEmitter::setFrontFaceEmit()
{
	SurfaceEmitter::setFrontFaceEmit();

	for(auto& emitter : m_emitters)
	{
		emitter.setFrontFaceEmit();
	}
}

void MultiDiffuseSurfaceEmitter::setBackFaceEmit()
{
	SurfaceEmitter::setBackFaceEmit();

	for(auto& emitter : m_emitters)
	{
		emitter.setBackFaceEmit();
	}
}

}// end namespace ph