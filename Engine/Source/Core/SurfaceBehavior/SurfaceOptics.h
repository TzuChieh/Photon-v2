#pragma once

#include "Core/SurfaceBehavior/surface_optics_fwd.h"
#include "Core/SurfaceBehavior/bsdf_query_fwd.h"
#include "Core/SurfaceBehavior/BsdfSample.h"

#include <string>
#include <array>

namespace ph
{

class SurfaceOptics
{
	// FIXME: sort-of hacked, should clarify the need of this in the future
	friend class LerpedSurfaceOptics;

public:
	SurfaceOptics();
	virtual ~SurfaceOptics();

	virtual ESurfacePhenomenon getPhenomenonOf(SurfaceElemental elemental) const = 0;

	void calcBsdf(BsdfEvalQuery& eval) const;
	void calcBsdfSample(BsdfSampleQuery& sample/*, std::array<real, 2> samples*/) const;
	void calcBsdfSamplePdfW(BsdfPdfQuery& pdfQuery) const;

	SurfacePhenomena getAllPhenomena() const;
	SurfaceElemental numElementals() const;

	virtual std::string toString() const;

protected:
	SurfacePhenomena m_phenomena;
	SurfaceElemental m_numElementals;

private:
	virtual void calcBsdf(
		const BsdfQueryContext& ctx,
		const BsdfEvalInput&    in,
		BsdfEvalOutput&         out) const = 0;

	virtual void calcBsdfSample(
		const BsdfQueryContext& ctx,
		const BsdfSampleInput&  in,
		BsdfSample              sample,
		BsdfSampleOutput&       out) const = 0;

	virtual void calcBsdfSamplePdfW(
		const BsdfQueryContext& ctx,
		const BsdfPdfInput&     in,
		BsdfPdfOutput&          out) const = 0;
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
