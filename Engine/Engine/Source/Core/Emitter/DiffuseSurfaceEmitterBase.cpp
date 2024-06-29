#include "Core/Emitter/DiffuseSurfaceEmitterBase.h"
#include "Math/TVector3.h"
#include "Core/Texture/constant_textures.h"
#include "Core/Intersection/Primitive.h"
#include "Core/Intersection/Query/PrimitivePosSampleQuery.h"
#include "Core/Intersection/Query/PrimitivePosPdfQuery.h"
#include "Core/Emitter/Query/DirectEnergySampleQuery.h"
#include "Core/Emitter/Query/DirectEnergyPdfQuery.h"
#include "Core/Emitter/Query/EnergyEmissionSampleQuery.h"
#include "Core/Ray.h"
#include "Core/HitProbe.h"
#include "Core/HitDetail.h"
#include "Core/Texture/TSampler.h"
#include "Math/constant.h"
#include "Math/Color/Spectrum.h"
#include "Core/Texture/SampleLocation.h"
#include "Math/Geometry/THemisphere.h"
#include "Math/TOrthonormalBasis3.h"
#include "Core/SampleGenerator/SampleFlow.h"
#include "Core/LTA/lta.h"
#include "Math/Color/color_spaces.h"

#include <limits>

namespace ph
{

DiffuseSurfaceEmitterBase::DiffuseSurfaceEmitterBase(
	const EmitterFeatureSet featureSet)

	: SurfaceEmitter(featureSet)
{}

void DiffuseSurfaceEmitterBase::evalEnergyFromSurface(
	const TTexture<math::Spectrum>& energy,
	const SurfaceHit& Xe,
	math::Spectrum* const out_energy) const
{
	const math::Vector3R emitDir = Xe.getIncidentRay().getDir().mul(-1.0_r);
	if(!canEmit(emitDir, Xe.getShadingNormal()))
	{
		out_energy->setColorValues(0.0_r);
		return;
	}

	// TODO: able to specify channel or restrict it
	TSampler<math::Spectrum> sampler(math::EColorUsage::EMR);
	*out_energy = sampler.sample(energy, Xe);
}

void DiffuseSurfaceEmitterBase::genDirectSampleFromSurface(
	const Primitive& surface,
	const TTexture<math::Spectrum>& energy,
	DirectEnergySampleQuery& query,
	SampleFlow& sampleFlow,
	HitProbe& probe) const
{
	if(getFeatureSet().hasNo(EEmitterFeatureSet::DirectSample))
	{
		return;
	}

	PrimitivePosSampleQuery posSample;
	posSample.inputs.set(query.inputs);
	surface.genPosSample(posSample, sampleFlow, probe);
	if(!posSample.outputs)
	{
		return;
	}

	HitDetail detail;
	HitProbe(probe).calcHitDetail(posSample.outputs.getObservationRay(), &detail);

	const auto emitterToTargetPos = query.inputs.getTargetPos() - posSample.outputs.getPos();
	if(!canEmit(emitterToTargetPos, detail.getShadingNormal()))
	{
		return;
	}

	math::Spectrum emittedEnergy;
	energy.sample(SampleLocation(detail, math::EColorUsage::EMR), &emittedEnergy);

	query.outputs.setEmitPos(posSample.outputs.getPos());
	query.outputs.setEmittedEnergy(emittedEnergy);
	query.outputs.setSrcPrimitive(&surface);
	query.outputs.setPdf(lta::PDF::W(lta::pdfA_to_pdfW(
		posSample.outputs.getPdfA(), emitterToTargetPos, detail.getShadingNormal())));
	query.outputs.setObservationRay(posSample.outputs.getObservationRay());
}

void DiffuseSurfaceEmitterBase::emitRayFromSurface(
	const Primitive& surface,
	const TTexture<math::Spectrum>& energy,
	EnergyEmissionSampleQuery& query,
	SampleFlow& sampleFlow,
	HitProbe& probe) const
{
	if(getFeatureSet().hasNo(EEmitterFeatureSet::EmissionSample))
	{
		return;
	}

	PrimitivePosSampleQuery posSample;
	posSample.inputs.set(query.inputs.getTime());
	surface.genPosSample(posSample, sampleFlow, probe);
	if(!posSample.outputs)
	{
		return;
	}

	real pdfW;
	auto localRayDir = math::THemisphere<real>::makeUnit().sampleToSurfaceCosThetaWeighted(
		sampleFlow.flow2D(), &pdfW);
	if(m_isBackFaceEmission)
	{
		localRayDir.negateLocal();
	}

	HitDetail detail;
	HitProbe(probe).calcHitDetail(posSample.outputs.getObservationRay(), &detail);

	const auto sampleBasis = math::Basis3R::makeFromUnitY(detail.getShadingNormal());
	const auto rayDir = sampleBasis.localToWorld(localRayDir);
	if(!canEmit(rayDir, detail.getShadingNormal()))
	{
		return;
	}

	const Ray emittedRay(
		posSample.outputs.getPos(),
		rayDir.normalize(),
		0,
		std::numeric_limits<real>::max(),
		query.inputs.getTime());

	// The emitted ray will be representing the hit event
	probe.replaceBaseHitRayTWith(emittedRay.getMinT());

	math::Spectrum emittedEnergy;
	energy.sample(SampleLocation(detail, math::EColorUsage::EMR), &emittedEnergy);

	query.outputs.setEmittedRay(emittedRay);
	query.outputs.setPdf(posSample.outputs.getPdfPos(), lta::PDF::W(pdfW));
	query.outputs.setEmittedEnergy(emittedEnergy);
}

}// end namespace ph
