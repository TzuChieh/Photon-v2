#pragma once

#include "Engine/Core/SurfaceBehavior/surface_optics_fwd.h"
#include "Engine/Core/SurfaceBehavior/bsdf_query_fwd.h"

#include <Common/compiler.h>
#include <Common/primitive_type.h>

#include <string>
#include <array>
#include <iterator>
#include <optional>

namespace ph
{

class SampleFlow;
class SurfaceHit;

/*! @brief Describes how light interacts with a surface. 
*/
class SurfaceOptics
{
public:
	class ElementalIterator;
	class ElementalIteratorProxy;

	SurfaceOptics();
	virtual ~SurfaceOptics() = default;

	/*! @brief Get the phenomenon of a surface component.
	One can also setup query for a specific elemental. See `BsdfQueryContext`.
	@param elemental The component to query. Must be within [0, `numElementals()`) and
	must not be `ALL_SURFACE_ELEMENTALS`. Use `getAllPhenomena()` if you want to query the phenomena
	for `ALL_SURFACE_ELEMENTALS`.
	*/
	virtual ESurfacePhenomenon getPhenomenonOf(SurfaceElemental elemental) const = 0;

	/*! @brief Calculate BSDF for all or one elemental.
	BSDF-based optics generally have 3 core methods: evaluation, sampling and PDF calculation.
	These methods must work consistently with each other, i.e., if a BSDF is sampled with a specific
	set of input, the evaluation and PDF calculation for that set of input, deterministically.
	Implementations do not need to care whether the input or output vectors are in a geometrically
	possible configuration (e.g., sidedness according to `ESidednessPolicy`).
	@param ctx The context of the query. It is assumed `BsdfQueryContext::targetPhenomena == ALL_SURFACE_PHENOMENA`
	for elemental-based query.
	*/
	virtual void calcElementalBsdf(
		const BsdfQueryContext& ctx,
		const BsdfEvalInput&    in,
		BsdfEvalOutput&         out) const = 0;

	/*! @brief Generate BSDF sample for all or one elemental.
	See `calcElementalBsdf()` for more information.
	*/
	virtual void genElementalBsdfSample(
		const BsdfQueryContext& ctx,
		const BsdfSampleInput&  in,
		SampleFlow&             sampleFlow,
		BsdfSampleOutput&       out) const = 0;

	/*! @brief Calculate BSDF sample PDF for all or one elemental.
	See `calcElementalBsdf()` for more information.
	*/
	virtual void calcElementalBsdfPdf(
		const BsdfQueryContext& ctx,
		const BsdfPdfInput&     in,
		BsdfPdfOutput&          out) const = 0;

	/*! @brief Generate BSDF sample for all or a subset of phenomena.
	This is the phenomenon variant of `calcElementalBsdf()`.
	@param ctx The context of the query. It is assumed `BsdfQueryContext::targetPhenomena != ALL_SURFACE_PHENOMENA`
	for phenomenon-based query.
	*/
	virtual void calcPhenomenalBsdf(
		const BsdfQueryContext& ctx,
		const BsdfEvalInput&    in,
		BsdfEvalOutput&         out) const;

	/*! @brief Generate BSDF sample for all or a subset of phenomena.
	This is the phenomenon variant of `genElementalBsdfSample()`. See `calcPhenomenalBsdf()` for more information.
	*/
	virtual void genPhenomenalBsdfSample(
		const BsdfQueryContext& ctx,
		const BsdfSampleInput&  in,
		SampleFlow&             sampleFlow,
		BsdfSampleOutput&       out) const;

	/*! @brief Calculate BSDF sample PDF for all or a subset of phenomena.
	This is the phenomenon variant of `calcElementalBsdfPdf()`. See `calcPhenomenalBsdf()` for more information.
	*/
	virtual void calcPhenomenalBsdfPdf(
		const BsdfQueryContext& ctx,
		const BsdfPdfInput&     in,
		BsdfPdfOutput&          out) const;

	virtual std::string toString() const;

	/*! @brief Executes a BSDF evaluation query.
	Respects sidedness policy.
	*/
	void calcBsdf(BsdfEvalQuery& eval) const;

	/*! @brief Executes a BSDF sample query.
	Respects sidedness policy.
	*/
	void genBsdfSample(BsdfSampleQuery& sample, SampleFlow& sampleFlow) const;

	/*! @brief Executes a BSDF sample PDF query.
	Respects sidedness policy.
	*/
	void calcBsdfPdf(BsdfPdfQuery& pdfQuery) const;

	/*! @brief Get all phenomena that exist in this surface.
	@note If a surface has any delta phenomenon along with any non-delta phenomenon, it should be
	treated the same way as a surface with only pure delta phenomenon.
	*/
	SurfacePhenomena getAllPhenomena() const;

	/*! @brief Get number of components this surface contains.
	@see `getPhenomenonOf()`.
	*/
	SurfaceElemental numElementals() const;

	auto beginElementalIteratorFor(SurfacePhenomena phenomena) const
	-> ElementalIterator;

	auto endElementalIterator() const
	-> ElementalIterator;

	auto getElemenalIteratorProxy(SurfacePhenomena phenomena) const
	-> ElementalIteratorProxy;

public:
	struct ElementalInfo
	{
		SurfaceElemental   elemental  = 0;
		ESurfacePhenomenon phenomenon = static_cast<ESurfacePhenomenon>(0);
	};

	class ElementalIterator
	{
	public:
		// Standard iterator traits
		using iterator_category = std::forward_iterator_tag;
		using value_type        = ElementalInfo;
		using difference_type   = std::ptrdiff_t;
		using pointer           = const ElementalInfo*;
		using reference         = const ElementalInfo&;

		// Default constructible
		ElementalIterator() = default;

		ElementalIterator(
			const SurfaceOptics* optics,
			SurfacePhenomena     target,
			const ElementalInfo& current)

			: m_optics (optics)
			, m_target (target)
			, m_current(current)
		{}

		// Dereferenceable
		reference operator * () const
		{
			PH_ASSERT(m_target != SurfacePhenomena{});
			PH_ASSERT_LT(m_current.elemental, getOptics().numElementals());
			PH_ASSERT(m_current.phenomenon == getOptics().getPhenomenonOf(m_current.elemental));
			PH_ASSERT(m_target.has(m_current.phenomenon));

			return m_current;
		}

		// Dereferenceable
		pointer operator -> () const
		{
			return &(this->operator * ());
		}

		// Pre-incrementable
		ElementalIterator& operator ++ ()
		{
			++m_current.elemental;
			m_current = getOptics().nextElementalOf(m_target, m_current.elemental);
			return *this;
		}

		// Post-incrementable
		ElementalIterator operator ++ (int)
		{
			ElementalIterator current = *this;
			++(*this);
			return current;
		}

		// Equality
		bool operator == (const ElementalIterator& rhs) const
		{
			// It's user's responsibility to not mix different target phenomena (but allow empty for shared end)
			PH_ASSERT(m_target == rhs.m_target || rhs.m_target == SurfacePhenomena{});

			return m_current.elemental == rhs.m_current.elemental && m_optics == rhs.m_optics;
		}

#if !PH_COMPILER_HAS_P2468R2
		// Inequality
		bool operator != (const ElementalIterator& rhs) const
		{
			return !(*this == rhs);
		}
#endif

	private:
		const SurfaceOptics& getOptics() const
		{
			PH_ASSERT(m_optics);
			return *m_optics;
		}

		const SurfaceOptics* m_optics  = nullptr;
		SurfacePhenomena     m_target  = ALL_SURFACE_PHENOMENA;
		ElementalInfo        m_current = {};
	};

	class ElementalIteratorProxy
	{
	public:
		ElementalIteratorProxy(const SurfaceOptics& optics, SurfacePhenomena phenomena)
			: m_optics   (optics)
			, m_phenomena(phenomena)
		{}

		ElementalIterator begin() const
		{
			return m_optics.beginElementalIteratorFor(m_phenomena);
		}

		ElementalIterator end() const
		{
			return m_optics.endElementalIterator();
		}

	private:
		const SurfaceOptics& m_optics;
		SurfacePhenomena     m_phenomena;
	};

protected:
	std::optional<SurfaceElemental> selectElementalFromKey(
		const BsdfQueryContext& ctx,
		real* out_pdf) const;

	SurfacePhenomena m_phenomena;
	SurfaceElemental m_numElementals;

private:
	ElementalInfo nextElementalOf(SurfacePhenomena phenomena, SurfaceElemental fromElemental) const
	{
		SurfaceElemental ei = fromElemental;
		while(ei < numElementals())
		{
			const ESurfacePhenomenon phenomenon = getPhenomenonOf(ei);
			if(phenomena.has(phenomenon))
			{
				return {ei, phenomenon};
			}

			++ei;
		}

		// Be consistent with the end iterator
		return {numElementals(), static_cast<ESurfacePhenomenon>(0)};
	}
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

inline auto SurfaceOptics::beginElementalIteratorFor(SurfacePhenomena phenomena) const
-> ElementalIterator
{
	return ElementalIterator(this, phenomena, nextElementalOf(phenomena, 0));
}

inline auto SurfaceOptics::endElementalIterator() const
-> ElementalIterator
{
	return ElementalIterator(this, SurfacePhenomena{}, ElementalInfo{numElementals(), static_cast<ESurfacePhenomenon>(0)});
}

inline auto SurfaceOptics::getElemenalIteratorProxy(SurfacePhenomena phenomena) const
-> ElementalIteratorProxy
{
	return ElementalIteratorProxy(*this, phenomena);
}

}// end namespace ph
