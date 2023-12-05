#pragma once

#include "Common/debug.h"

#include <cstddef>
#include <cstdint>
#include <memory>

namespace ph
{

/*!
A simple stack memory corruption detector.
Reference: https://blog.unity.com/technology/debugging-memory-corruption-who-the-hell-writes-2-into-my-stack-2
*/
template<std::size_t BUFFER_BYTES = 16 * 1024>
class TStackSentinel final
{
public:
	static auto makeUnique()
	-> std::unique_ptr<TStackSentinel>;

	TStackSentinel();
	~TStackSentinel();

private:
	static std::uint8_t genValue(std::size_t byteIdx);

	std::uint8_t m_stackBuffer[BUFFER_BYTES];
};

template<std::size_t BUFFER_BYTES>
inline auto TStackSentinel<BUFFER_BYTES>::makeUnique()
-> std::unique_ptr<TStackSentinel>
{
	return std::make_unique<TStackSentinel>();
}

template<std::size_t BUFFER_BYTES>
inline TStackSentinel<BUFFER_BYTES>::TStackSentinel()
{
	for(std::size_t i = 0; i < BUFFER_BYTES; ++i)
	{
		m_stackBuffer[i] = genValue(i);
	}
}

template<std::size_t BUFFER_BYTES>
inline TStackSentinel<BUFFER_BYTES>::~TStackSentinel()
{
	for(std::size_t i = 0; i < BUFFER_BYTES; ++i)
	{
		if(m_stackBuffer[i] != genValue(i))
		{
			PH_DEBUG_BREAK();
		}
	}
}

template<std::size_t BUFFER_BYTES>
inline std::uint8_t TStackSentinel<BUFFER_BYTES>::genValue(const std::size_t byteIdx)
{
	// Generates the pattern `BAADF00D` (bad food)
	switch(byteIdx % 4)
	{
	case 0: return 0xBA;
	case 1: return 0xAD;
	case 2: return 0xF0;
	case 3: return 0x0D;
	}
}

}// end namespace ph
