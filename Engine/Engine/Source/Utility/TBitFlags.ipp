#pragma once

#include "Utility/TBitFlags.h"

namespace ph
{

template<typename Value, typename Input>
inline constexpr TBitFlags<Value, Input>::TBitFlags() :
	m_bits(0)
{}

template<typename Value, typename Input>
inline constexpr TBitFlags<Value, Input>::TBitFlags(const FlagsSet& flagsSet) :
	TBitFlags(static_cast<Input>(collectFlags(flagsSet)))
{}

template<typename Value, typename Input>
inline constexpr TBitFlags<Value, Input>::TBitFlags(const Input flagsSet) :
	m_bits(static_cast<Value>(flagsSet))
{}

template<typename Value, typename Input>
template<typename OtherInput>
inline constexpr TBitFlags<Value, Input>::TBitFlags(const TBitFlags<Value, OtherInput>& otherFlags) :
	m_bits(otherFlags.get())
{}

template<typename Value, typename Input>
inline constexpr TBitFlags<Value, Input>& TBitFlags<Value, Input>::unionWith(const FlagsSet& flagsSet)
{
	TBitFlags flags;
	flags.m_bits = collectFlags(flagsSet);
	return unionWith(flags);
}

template<typename Value, typename Input>
inline constexpr TBitFlags<Value, Input>& TBitFlags<Value, Input>::unionWith(const TBitFlags& flags)
{
	m_bits |= flags.m_bits;

	return *this;
}

template<typename Value, typename Input>
inline constexpr TBitFlags<Value, Input>& TBitFlags<Value, Input>::intersectWith(const FlagsSet& flagsSet)
{
	m_bits &= collectFlags(flagsSet);

	return *this;
}

template<typename Value, typename Input>
inline constexpr TBitFlags<Value, Input>& TBitFlags<Value, Input>::set(const FlagsSet& flagsSet)
{
	return set(static_cast<Input>(collectFlags(flagsSet)));
}

template<typename Value, typename Input>
inline constexpr TBitFlags<Value, Input>& TBitFlags<Value, Input>::set(const Input flagsSet)
{
	m_bits = static_cast<Value>(flagsSet);
	return *this;
}

template<typename Value, typename Input>
inline constexpr TBitFlags<Value, Input>& TBitFlags<Value, Input>::turnOn(const FlagsSet& flagsSet)
{
	return unionWith(flagsSet);
}

template<typename Value, typename Input>
inline constexpr TBitFlags<Value, Input>& TBitFlags<Value, Input>::turnOff(const FlagsSet& flagsSet)
{
	m_bits &= ~(collectFlags(flagsSet));

	return *this;
}

template<typename Value, typename Input>
inline constexpr bool TBitFlags<Value, Input>::hasNone(const FlagsSet& flagsSet) const
{
	return hasNone(static_cast<Input>(collectFlags(flagsSet)));
}

template<typename Value, typename Input>
inline constexpr bool TBitFlags<Value, Input>::hasAny(const FlagsSet& flagsSet) const
{
	return hasAny(static_cast<Input>(collectFlags(flagsSet)));
}

template<typename Value, typename Input>
inline constexpr bool TBitFlags<Value, Input>::hasAll(const FlagsSet& flagsSet) const
{
	return hasAll(static_cast<Input>(collectFlags(flagsSet)));
}

template<typename Value, typename Input>
inline constexpr bool TBitFlags<Value, Input>::hasExactly(const FlagsSet& flagsSet) const
{
	return hasExactly(static_cast<Input>(collectFlags(flagsSet)));
}

template<typename Value, typename Input>
inline constexpr bool TBitFlags<Value, Input>::hasNone(const Input flagsSet) const
{
	const auto flagsValue = static_cast<Value>(flagsSet);
	return (m_bits & flagsValue) == 0;
}

template<typename Value, typename Input>
inline constexpr bool TBitFlags<Value, Input>::hasAny(const Input flagsSet) const
{
	const auto flagsValue = static_cast<Value>(flagsSet);
	return (m_bits & flagsValue) != 0;
}

template<typename Value, typename Input>
inline constexpr bool TBitFlags<Value, Input>::hasAll(const Input flagsSet) const
{
	const auto flagsValue = static_cast<Value>(flagsSet);
	return (m_bits & flagsValue) == flagsValue;
}

template<typename Value, typename Input>
inline constexpr bool TBitFlags<Value, Input>::hasExactly(const Input flagsSet) const
{
	const auto flagsValue = static_cast<Value>(flagsSet);
	return m_bits == flagsValue;
}

template<typename Value, typename Input>
inline constexpr bool TBitFlags<Value, Input>::has(const Input singleFlag) const
{
	return hasAll(singleFlag);
}

template<typename Value, typename Input>
inline constexpr bool TBitFlags<Value, Input>::hasNo(const Input singleFlag) const
{
	return hasNone(singleFlag);
}

template<typename Value, typename Input>
inline constexpr bool TBitFlags<Value, Input>::isEmpty() const
{
	return m_bits == 0;
}

template<typename Value, typename Input>
inline constexpr bool TBitFlags<Value, Input>::isEqual(const TBitFlags& other) const
{
	return m_bits == other.m_bits;
}

template<typename Value, typename Input>
inline constexpr Value TBitFlags<Value, Input>::get() const
{
	return m_bits;
}

template<typename Value, typename Input>
inline constexpr Value TBitFlags<Value, Input>::collectFlags(const FlagsSet& flagsSet)
{
	Value inputFlags = 0;
	for(const Input& flag : flagsSet)
	{
		inputFlags |= static_cast<Value>(flag);
	}

	return inputFlags;
}

}// end namespace ph
