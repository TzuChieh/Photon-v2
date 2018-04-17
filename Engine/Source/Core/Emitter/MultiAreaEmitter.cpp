#include "Core/Emitter/MultiAreaEmitter.h"
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

MultiAreaEmitter::MultiAreaEmitter(const std::vector<PrimitiveAreaEmitter>& areaEmitters) :
	SurfaceEmitter(),
	m_areaEmitters()
{
	PH_ASSERT(!areaEmitters.empty());

	m_extendedArea = 0.0_r;
	for(const auto& areaEmitter : areaEmitters)
	{
		const Primitive* primitive = areaEmitter.getPrimitive();

		PH_ASSERT(primitive != nullptr);
		m_extendedArea += primitive->calcExtendedArea();

		addEmitter(areaEmitter);
	}
	m_reciExtendedArea = 1.0_r / m_extendedArea;
}

MultiAreaEmitter::~MultiAreaEmitter() = default;

//static int iii = 0;
void MultiAreaEmitter::evalEmittedRadiance(const SurfaceHit& X, SpectralStrength* out_radiance) const
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

void MultiAreaEmitter::genDirectSample(DirectLightSample& sample) const
{
	const PrimitiveAreaEmitter& emitter = m_areaEmitters[Random::genUniformIndex_iL_eU(0, m_areaEmitters.size())];

	emitter.genDirectSample(sample);
	const real pickPdf = (1.0_r / static_cast<real>(m_areaEmitters.size()));
	sample.pdfW *= pickPdf;
}

void MultiAreaEmitter::genSensingRay(Ray* out_ray, SpectralStrength* out_Le, Vector3R* out_eN, real* out_pdfA, real* out_pdfW) const
{
	// TODO
	PH_ASSERT_UNREACHABLE_SECTION();
}

real MultiAreaEmitter::calcDirectSamplePdfW(const Vector3R& targetPos, const Vector3R& emitPos, const Vector3R& emitN, const Primitive* hitPrim) const
{
	// HACK

	const Vector3R emitDir(targetPos.sub(emitPos).normalizeLocal());
	if(!canEmit(emitDir, emitN))
	{
		return 0.0_r;
	}
	
	const real emitDirDotNormal = emitDir.dot(emitN);
	const real pickPdf = (1.0_r / static_cast<real>(m_areaEmitters.size()));
	const real samplePdfA  = hitPrim->calcPositionSamplePdfA(emitPos);
	const real distSquared = targetPos.sub(emitPos).lengthSquared();
	return samplePdfA / std::abs(emitDirDotNormal) * distSquared * pickPdf;
}

void MultiAreaEmitter::setEmittedRadiance(const std::shared_ptr<TTexture<SpectralStrength>>& emittedRadiance)
{
	SurfaceEmitter::setEmittedRadiance(emittedRadiance);

	for(auto& areaEmitter : m_areaEmitters)
	{
		areaEmitter.setEmittedRadiance(emittedRadiance);
	}
}

void MultiAreaEmitter::addEmitter(const PrimitiveAreaEmitter& emitter)
{
	m_areaEmitters.push_back(emitter);

	if(m_isBackFaceEmission)
	{
		m_areaEmitters.back().setBackFaceEmit();
	}
	else
	{
		m_areaEmitters.back().setFrontFaceEmit();
	}
}

void MultiAreaEmitter::setFrontFaceEmit()
{
	SurfaceEmitter::setFrontFaceEmit();

	for(auto& emitter : m_areaEmitters)
	{
		emitter.setFrontFaceEmit();
	}
}

void MultiAreaEmitter::setBackFaceEmit()
{
	SurfaceEmitter::setBackFaceEmit();

	for(auto& emitter : m_areaEmitters)
	{
		emitter.setBackFaceEmit();
	}
}

}// end namespace ph