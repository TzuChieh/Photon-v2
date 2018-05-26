#pragma once

#include "Utility/TBitFlags.h"

namespace ph
{

template<typename Value, typename Input>
inline TBitFlags<Value, Input>::TBitFlags() : 
	m_bits(0)
{}

template<typename Value, typename Input>
inline TBitFlags<Value, Input>& TBitFlags<Value, Input>::unionWith(const FlagsSet& flagsSet)
{
	TBitFlags flags;
	flags.m_bits = collectFlags(flagsSet);
	return unionWith(flags);
}

template<typename Value, typename Input>
inline TBitFlags<Value, Input>& TBitFlags<Value, Input>::unionWith(const TBitFlags& flags)
{
	m_bits |= flags.m_bits;

	return *this;
}

template<typename Value, typename Input>
inline TBitFlags<Value, Input>& TBitFlags<Value, Input>::intersectWith(const FlagsSet& flagsSet)
{
	m_bits &= collectFlags(flagsSet);

	return *this;
}

template<typename Value, typename Input>
inline TBitFlags<Value, Input>& TBitFlags<Value, Input>::set(const FlagsSet& flagsSet)
{
	m_bits = collectFlags(flagsSet);

	return *this;
}

template<typename Value, typename Input>
inline bool TBitFlags<Value, Input>::hasNone(const FlagsSet& flagsSet) const
{
	return (m_bits & collectFlags(flagsSet)) == 0;
}

template<typename Value, typename Input>
inline bool TBitFlags<Value, Input>::hasAtLeastOne(const FlagsSet& flagsSet) const
{
	return (m_bits & collectFlags(flagsSet)) != 0;
}

template<typename Value, typename Input>
inline bool TBitFlags<Value, Input>::hasAll(const FlagsSet& flagsSet) const
{
	const Value& inputFlags = collectFlags(flagsSet);
	return (m_bits & inputFlags) == inputFlags;
}

template<typename Value, typename Input>
inline bool TBitFlags<Value, Input>::hasExactly(const FlagsSet& flagsSet) const
{
	return m_bits == collectFlags(flagsSet);
}

template<typename Value, typename Input>
inline bool TBitFlags<Value, Input>::isEmpty() const
{
	return m_bits == 0;
}

template<typename Value, typename Input>
inline bool TBitFlags<Value, Input>::equals(const TBitFlags& other) const
{
	return m_bits == other.m_bits;
}

template<typename Value, typename Input>
inline Value TBitFlags<Value, Input>::collectFlags(const FlagsSet& flagsSet)
{
	Value inputFlags = 0;
	for(const auto& flags : flagsSet)
	{
		inputFlags |= static_cast<Value>(flags);
	}

	return inputFlags;
}

}// end namespace ph