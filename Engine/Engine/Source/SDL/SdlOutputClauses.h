#pragma once

#include "SDL/SdlOutputClause.h"

#include <Common/assertion.h>

#include <cstddef>
#include <vector>

namespace ph
{

class SdlOutputClauses final
{
public:
	SdlOutputClauses();

	SdlOutputClause& createClause();
	void clear();

	std::size_t numClauses() const;
	const SdlOutputClause& operator [] (std::size_t clauseIdx) const;

private:
	std::vector<SdlOutputClause> m_clauseBuffer;
	std::size_t m_numUsedClauses;
};

inline SdlOutputClauses::SdlOutputClauses() :
	m_clauseBuffer(),
	m_numUsedClauses(0)
{}

inline SdlOutputClause& SdlOutputClauses::createClause()
{
	// Increase buffer size if there is not enough clauses
	if(m_numUsedClauses == m_clauseBuffer.size())
	{
		m_clauseBuffer.push_back(SdlOutputClause());
	}

	PH_ASSERT_LT(m_numUsedClauses, m_clauseBuffer.size());
	SdlOutputClause& newClause = m_clauseBuffer[m_numUsedClauses++];

	// Clear clause data as it may be used
	newClause.clear();

	return newClause;
}

inline void SdlOutputClauses::clear()
{
	m_numUsedClauses = 0;
}

inline std::size_t SdlOutputClauses::numClauses() const
{
	PH_ASSERT_LE(m_numUsedClauses, m_clauseBuffer.size());
	return m_numUsedClauses;
}

inline const SdlOutputClause& SdlOutputClauses::operator [] (const std::size_t clauseIdx) const
{
	PH_ASSERT_LT(clauseIdx, numClauses());
	return m_clauseBuffer[clauseIdx];
}

}// end namespace ph
