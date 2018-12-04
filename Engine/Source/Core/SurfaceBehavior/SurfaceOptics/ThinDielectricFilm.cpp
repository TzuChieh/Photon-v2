#include "Core/SurfaceBehavior/SurfaceOptics/ThinDielectricFilm.h"
#include "Core/SurfaceBehavior/Property/ExactDielectricFresnel.h"
#include "Common/assertion.h"
#include "Math/Random.h"
#include "Core/LTABuildingBlock/SidednessAgreement.h"
#include "Core/Texture/TConstantTexture.h"
#include "Core/Texture/TSampler.h"
#include "Math/math.h"

namespace ph
{

ThinDielectricFilm::ThinDielectricFilm(
	const std::vector<SampledSpectralStrength>& reflectanceTable,
	const std::vector<SampledSpectralStrength>& transmittanceTable) :

	SurfaceOptics(),

	m_reflectanceTable(reflectanceTable),
	m_transmittanceTable(transmittanceTable)
{
	PH_ASSERT_EQ(reflectanceTable.size(), 91);
	PH_ASSERT_EQ(transmittanceTable.size(), 91);

	m_phenomena.set({ESurfacePhenomenon::DELTA_REFLECTION, ESurfacePhenomenon::DELTA_TRANSMISSION});
	m_numElementals = 2;
}

ESurfacePhenomenon ThinDielectricFilm::getPhenomenonOf(const SurfaceElemental elemental) const
{
	PH_ASSERT_LT(elemental, 2);

	return elemental == REFLECTION ? ESurfacePhenomenon::DELTA_REFLECTION : 
	                                 ESurfacePhenomenon::DELTA_TRANSMISSION;
}

void ThinDielectricFilm::calcBsdf(
	const BsdfEvaluation::Input& in,
	BsdfEvaluation::Output&      out,
	const SidednessAgreement&    sidedness) const
{
	out.bsdf.setValues(0.0_r);
}

void ThinDielectricFilm::calcBsdfSample(
	const BsdfSample::Input&  in,
	BsdfSample::Output&       out,
	const SidednessAgreement& sidedness) const
{
	const bool canReflect  = in.elemental == ALL_ELEMENTALS || in.elemental == REFLECTION;
	const bool canTransmit = in.elemental == ALL_ELEMENTALS || in.elemental == TRANSMISSION;

	if(!canReflect && !canTransmit)
	{
		out.setValidity(false);
		return;
	}

	const Vector3R& N = in.X.getShadingNormal();

	const Vector3R reflectDir  = in.V.mul(-1.0_r).reflect(N).normalizeLocal();
	const Vector3R transmitDir = in.V.mul(-1.0_r);

	const real        cosIncident     = std::min(N.absDot(reflectDir), 1.0_r);
	const real        incidentDegrees = math::to_degrees(std::acos(cosIncident));
	const std::size_t tableIndex      = math::clamp(
		static_cast<std::size_t>(incidentDegrees + 0.5_r), std::size_t(0), std::size_t(90));

	const SampledSpectralStrength& reflectance   = m_reflectanceTable[tableIndex];
	const SampledSpectralStrength& transmittance = m_transmittanceTable[tableIndex];

	const real reflectFactor  = reflectance.avg();
	const real transmitFactor = transmittance.avg();
	if(reflectFactor + transmitFactor <= 0.0_r)
	{
		out.setValidity(false);
		return;
	}
	const real reflectProb = reflectFactor / (reflectFactor + transmitFactor);

	bool sampleReflect  = canReflect;
	bool sampleTransmit = canTransmit;

	// we cannot sample both path, choose one randomly
	if(sampleReflect && sampleTransmit)
	{
		const real dart = Random::genUniformReal_i0_e1();
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

	SampledSpectralStrength energyScale(0);
	if(sampleReflect)
	{
		if(!sidedness.isSameHemisphere(in.X, in.V, reflectDir))
		{
			out.setValidity(false);
			return;
		}

		out.L = reflectDir;
		energyScale = reflectance;

		// account for probability
		if(in.elemental == ALL_ELEMENTALS)
		{
			energyScale.divLocal(reflectProb);
		}
	}
	else if(sampleTransmit)
	{
		if(!sidedness.isOppositeHemisphere(in.X, in.V, transmitDir))
		{
			out.setValidity(false);
			return;
		}

		out.L = transmitDir;
		energyScale = transmittance;

		/*if(in.transported == ETransport::RADIANCE)
		{
			real etaI = m_fresnel->getIorOuter();
			real etaT = m_fresnel->getIorInner();
			if(N.dot(out.L) < 0.0_r)
			{
				std::swap(etaI, etaT);
			}
			F.mulLocal(etaT * etaT / (etaI * etaI));
		}*/

		// account for probability
		if(in.elemental == ALL_ELEMENTALS)
		{
			energyScale.divLocal(1.0_r - reflectProb);
		}
	}
	else
	{
		// RARE: may be called due to numerical error
		out.setValidity(false);
		return;
	}

	SpectralStrength value;
	value.setSampled(energyScale / N.absDot(out.L));
	out.pdfAppliedBsdf.setValues(value);
	out.setValidity(true);
}

void ThinDielectricFilm::calcBsdfSamplePdfW(
	const BsdfPdfQuery::Input& in,
	BsdfPdfQuery::Output&      out,
	const SidednessAgreement&  sidedness) const
{
	out.sampleDirPdfW = 0.0_r;
}

}// end namespace ph