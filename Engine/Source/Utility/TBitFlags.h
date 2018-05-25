#pragma once

#include <type_traits>
#include <initializer_list>

namespace ph
{

template<typename Value, typename Input = Value>
class TBitFlags final
{
	static_assert(std::is_integral_v<Value>);
	static_assert(std::is_integral_v<Input> || std::is_enum_v<Input>);
	static_assert(sizeof(Value) == sizeof(Input));

private:
	typedef std::initializer_list<Input> FlagsSet;

public:
	// Creats an instance with no flags.
	//
	TBitFlags();

	void unionWith(const FlagsSet& flagsSet);
	void intersectWith(const FlagsSet& flagsSet);
	void set(const FlagsSet& flagsSet);

	bool hasNone(const FlagsSet& flagsSet) const;
	bool hasAtLeastOne(const FlagsSet& flagsSet) const;
	bool hasAll(const FlagsSet& flagsSet) const;
	bool hasExactly(const FlagsSet& flagsSet) const;
	
	bool isEmpty() const;
	bool equals(const TBitFlags& other) const;

	// TODO: clear all flags or clear specific ones

private:
	Value m_bits;

	static Value collectFlags(const FlagsSet& flagsSet);
};

}// end namespace ph

#include "Utility/TBitFlags.ipp"