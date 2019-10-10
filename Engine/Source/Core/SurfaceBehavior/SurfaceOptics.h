#pragma once

#include "Common/primitive_type.h"
#include "Math/TVector3.h"
#include "Core/SurfaceBehavior/surface_optics_fwd.h"
#include "Core/Quantity/SpectralStrength.h"
#include "Core/SurfaceHit.h"
#include "Core/SurfaceBehavior/BsdfEvaluation.h"
#include "Core/SurfaceBehavior/BsdfSample.h"
#include "Core/SurfaceBehavior/BsdfPdfQuery.h"

#include <string>

namespace ph
{

class Ray;
class SidednessAgreement;

class SurfaceOptics
{
	// FIXME: sort-of hacked, should clarify the need of this in the future
	friend class LerpedSurfaceOptics;

public:
	SurfaceOptics();
	virtual ~SurfaceOptics();

	virtual ESurfacePhenomenon getPhenomenonOf(SurfaceElemental elemental) const = 0;

	void calcBsdf(BsdfEvaluation& eval) const;
	void calcBsdfSample(BsdfSample& sample) const;
	void calcBsdfSamplePdfW(BsdfPdfQuery& pdfQuery) const;

	SurfacePhenomena getAllPhenomena() const;
	SurfaceElemental numElementals() const;

	virtual std::string toString() const;

protected:
	SurfacePhenomena m_phenomena;
	SurfaceElemental m_numElementals;

private:
	virtual void calcBsdf(
		const BsdfEvaluation::Input& in,
		BsdfEvaluation::Output&      out,
		const SidednessAgreement&    sidedness) const = 0;

	virtual void calcBsdfSample(
		const BsdfSample::Input&     in,
		BsdfSample::Output&          out,
		const SidednessAgreement&    sidedness) const = 0;

	virtual void calcBsdfSamplePdfW(
		const BsdfPdfQuery::Input&   in,
		BsdfPdfQuery::Output&        out,
		const SidednessAgreement&    sidedness) const = 0;
};

// In-header Implementations:

inline SurfacePhenomena SurfaceOptics::getAllPhenomena() const
{
	return m_phenomena;
}

inline SurfaceElemental SurfaceOptics::numElementals() const
{
	return m_numElementals;
}

inline std::string SurfaceOptics::toString() const
{
	return std::to_string(m_numElementals) + " elementals";
}

}// end namespace ph
