#pragma once

#include "SDL/SdlOutputPayload.h"
#include "Common/assertion.h"

#include <cstddef>
#include <vector>

namespace ph
{

class OutputPayloads final
{
public:
	OutputPayloads();

	SdlOutputPayload& createPayload();
	void clear();

	std::size_t numPayloads() const;
	const SdlOutputPayload& operator [] (std::size_t payloadIdx) const;

private:
	std::vector<SdlOutputPayload> m_payloadBuffer;
	std::size_t                   m_numUsedPayloads;
};

// In-header Implementations:

inline OutputPayloads::OutputPayloads() :
	m_payloadBuffer  (),
	m_numUsedPayloads(0)
{}

inline SdlOutputPayload& OutputPayloads::createPayload()
{
	// Increase buffer size if there is not enough payloads
	if(m_numUsedPayloads == m_payloadBuffer.size())
	{
		m_payloadBuffer.push_back(SdlOutputPayload());
	}

	PH_ASSERT_LT(m_numUsedPayloads, m_payloadBuffer.size());
	SdlOutputPayload& newPayload = m_payloadBuffer[m_numUsedPayloads++];

	// Clear payload data as it may be used
	newPayload.clear();

	return newPayload;
}

inline void OutputPayloads::clear()
{
	m_numUsedPayloads = 0;
}

inline std::size_t OutputPayloads::numPayloads() const
{
	PH_ASSERT_LE(m_numUsedPayloads, m_payloadBuffer.size());
	return m_numUsedPayloads;
}

inline const SdlOutputPayload& OutputPayloads::operator [] (const std::size_t payloadIdx) const
{
	PH_ASSERT_LT(payloadIdx, numPayloads());
	return m_payloadBuffer[payloadIdx];
}

}// end namespace ph
