#include "Core/SurfaceBehavior/SurfaceOptics/IdealDielectric.h"
#include "Core/SurfaceBehavior/Property/ExactDielectricFresnel.h"
#include "Common/assertion.h"
#include "Math/Random.h"
#include "Core/LTABuildingBlock/SidednessAgreement.h"
#include "Core/Texture/TConstantTexture.h"
#include "Core/Texture/TSampler.h"

namespace ph
{

IdealDielectric::IdealDielectric(const std::shared_ptr<DielectricFresnel>& fresnel) :

	IdealDielectric(
		fresnel, 
		std::make_shared<TConstantTexture<SpectralStrength>>(SpectralStrength(1.0_r)),
		std::make_shared<TConstantTexture<SpectralStrength>>(SpectralStrength(1.0_r)))
{}

IdealDielectric::IdealDielectric(
	const std::shared_ptr<DielectricFresnel>&          fresnel,
	const std::shared_ptr<TTexture<SpectralStrength>>& reflectionScale,
	const std::shared_ptr<TTexture<SpectralStrength>>& transmissionScale) : 

	SurfaceOptics(),

	m_fresnel(fresnel),
	m_reflectionScale(reflectionScale),
	m_transmissionScale(transmissionScale)
{
	PH_ASSERT(fresnel);
	PH_ASSERT(reflectionScale);
	PH_ASSERT(transmissionScale);

	m_phenomena.set({ESurfacePhenomenon::DELTA_REFLECTION, ESurfacePhenomenon::DELTA_TRANSMISSION});
	m_numElementals = 2;
}

ESurfacePhenomenon IdealDielectric::getPhenomenonOf(const SurfaceElemental elemental) const
{
	PH_ASSERT_LT(elemental, 2);

	return elemental == REFLECTION ? ESurfacePhenomenon::DELTA_REFLECTION : 
	                                 ESurfacePhenomenon::DELTA_TRANSMISSION;
}

void IdealDielectric::calcBsdf(
	const BsdfEvaluation::Input& in,
	BsdfEvaluation::Output&      out,
	const SidednessAgreement&    sidedness) const
{
	out.bsdf.setValues(0.0_r);
}

void IdealDielectric::calcBsdfSample(
	const BsdfSample::Input&  in,
	BsdfSample::Output&       out,
	const SidednessAgreement& sidedness) const
{
	const bool canReflect  = in.elemental == ALL_ELEMENTALS || in.elemental == REFLECTION;
	const bool canTransmit = in.elemental == ALL_ELEMENTALS || in.elemental == TRANSMISSION;

	if(!canReflect && !canTransmit)
	{
		out.setMeasurability(false);
		return;
	}

	const math::Vector3R N = in.X.getShadingNormal();

	SpectralStrength F;
	m_fresnel->calcReflectance(N.dot(in.V), &F);
	const real reflectProb = F.avg();

	bool sampleReflect  = canReflect;
	bool sampleTransmit = canTransmit;

	// we cannot sample both path, choose one randomly
	if(sampleReflect && sampleTransmit)
	{
		const real dart = math::Random::genUniformReal_i0_e1();
		if(dart < reflectProb)
		{
			sampleTransmit = false;
		}
		else
		{
			sampleReflect = false;
		}
	}

	PH_ASSERT(sampleReflect || sampleTransmit);

	if(sampleReflect)
	{
		// calculate reflected L
		out.L = in.V.mul(-1.0_r).reflect(N).normalizeLocal();
		if(!sidedness.isSameHemisphere(in.X, in.V, out.L))
		{
			out.setMeasurability(false);
			return;
		}

		// a scale factor for artistic control
		const SpectralStrength& reflectionScale =
			TSampler<SpectralStrength>(EQuantity::RAW).sample(*m_reflectionScale, in.X);
		F.mulLocal(reflectionScale);

		// account for probability
		if(in.elemental == ALL_ELEMENTALS)
		{
			F.divLocal(reflectProb);
		}
	}
	else if(sampleTransmit && m_fresnel->calcRefractDir(in.V, N, &(out.L)))
	{
		if(!sidedness.isOppositeHemisphere(in.X, in.V, out.L))
		{
			out.setMeasurability(false);
			return;
		}

		// FIXME: just use 1 - F
		m_fresnel->calcTransmittance(N.dot(out.L), &F);

		if(in.transported == ETransport::RADIANCE)
		{
			real etaI = m_fresnel->getIorOuter();
			real etaT = m_fresnel->getIorInner();
			if(N.dot(out.L) < 0.0_r)
			{
				std::swap(etaI, etaT);
			}
			F.mulLocal(etaT * etaT / (etaI * etaI));
		}

		// a scale factor for artistic control
		const SpectralStrength& transmissionScale =
			TSampler<SpectralStrength>(EQuantity::RAW).sample(*m_transmissionScale, in.X);
		F.mulLocal(transmissionScale);

		// account for probability
		if(in.elemental == ALL_ELEMENTALS)
		{
			F.divLocal(1.0_r - reflectProb);
		}
	}
	else
	{
		// RARE: may be called due to numerical error
		out.setMeasurability(false);
		return;
	}

	out.pdfAppliedBsdf.setValues(F / N.absDot(out.L));
	out.setMeasurability(true);
}

void IdealDielectric::calcBsdfSamplePdfW(
	const BsdfPdfQuery::Input& in,
	BsdfPdfQuery::Output&      out,
	const SidednessAgreement&  sidedness) const
{
	out.sampleDirPdfW = 0.0_r;
}

}// end namespace ph
