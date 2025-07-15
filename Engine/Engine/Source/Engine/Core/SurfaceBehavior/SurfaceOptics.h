#pragma once

#include "Engine/Core/SurfaceBehavior/surface_optics_fwd.h"
#include "Engine/Core/SurfaceBehavior/bsdf_query_fwd.h"

#include <Common/compiler.h>

#include <string>
#include <array>
#include <iterator>

namespace ph
{

class SampleFlow;

/*! @brief Describes how light interacts with a surface. 
*/
class SurfaceOptics
{
public:
	class ElementalIterator;

	SurfaceOptics();
	virtual ~SurfaceOptics() = default;

	/*! @brief Get the phenomenon of a surface component.
	One can also setup query for a specific elemental. See `BsdfQueryContext`.
	@param elemental The component to query. Must be within [0, `numElementals()`) and
	must not be `ALL_SURFACE_ELEMENTALS`. Use `getAllPhenomena()` if you want to query the phenomena
	for `ALL_SURFACE_ELEMENTALS`.
	*/
	virtual ESurfacePhenomenon getPhenomenonOf(SurfaceElemental elemental) const = 0;

	/*! @brief Calculate BSDF.
	Implementations do not need to care whether the input or output vectors are in a geometrically
	possible configuration (e.g., sidedness according to `ESidednessPolicy`).
	*/
	virtual void calcBsdfCore(
		const BsdfQueryContext& ctx,
		const BsdfEvalInput&    in,
		BsdfEvalOutput&         out) const = 0;

	/*! @brief Generate BSDF sample.
	Implementations do not need to care whether the input or output vectors are in a geometrically
	possible configuration (e.g., sidedness according to `ESidednessPolicy`).
	*/
	virtual void genBsdfSampleCore(
		const BsdfQueryContext& ctx,
		const BsdfSampleInput&  in,
		SampleFlow&             sampleFlow,
		BsdfSampleOutput&       out) const = 0;

	/*! @brief Calculate BSDF PDF.
	Implementations do not need to care whether the input or output vectors are in a geometrically
	possible configuration (e.g., sidedness according to `ESidednessPolicy`).
	*/
	virtual void calcBsdfPdfCore(
		const BsdfQueryContext& ctx,
		const BsdfPdfInput&     in,
		BsdfPdfOutput&          out) const = 0;

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

	auto getElementalIteratorFor(SurfacePhenomena phenomena) const
	-> ElementalIterator;

	// TODO: proxy for ranged for

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
			PH_ASSERT(m_optics);
			PH_ASSERT_LT(m_current.elemental, m_optics->numElementals());
			PH_ASSERT(m_current.phenomenon == m_optics->getPhenomenonOf(m_current.elemental));
			PH_ASSERT(m_target.has(m_current.phenomenon));

			return m_current;
		}

		// Pre-incrementable
		ElementalIterator& operator ++ ()
		{
			PH_ASSERT(m_optics);

			++m_current.elemental;
			m_current = m_optics->nextElementalOf(m_target, m_current.elemental);
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
		const SurfaceOptics* m_optics  = nullptr;
		SurfacePhenomena     m_target  = ALL_SURFACE_PHENOMENA;
		ElementalInfo        m_current = {};
	};

protected:
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

inline auto SurfaceOptics::getElementalIteratorFor(SurfacePhenomena phenomena) const
-> ElementalIterator
{
	return ElementalIterator(this, phenomena, nextElementalOf(phenomena, 0));
}

}// end namespace ph
