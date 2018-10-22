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
	TBitFlags();

	// Unions specified flags into this instance.
	TBitFlags& unionWith(const FlagsSet& flagsSet);
	TBitFlags& unionWith(const TBitFlags& flags);

	// Intersects this instance with the specified flags.
	TBitFlags& intersectWith(const FlagsSet& flagsSet);
	
	TBitFlags& set(const FlagsSet& flagsSet);

	// Checks whether this instance contains no specified flags.
	bool hasNone(const FlagsSet& flagsSet) const;

	// Checks whether this instance contains at least one of the specified flags.
	bool hasAtLeastOne(const FlagsSet& flagsSet) const;

	// Checks whether this instance contains all of the specified flags.
	bool hasAll(const FlagsSet& flagsSet) const;

	// Checks whether this instance contains exactly the specified flags.
	bool hasExactly(const FlagsSet& flagsSet) const;
	
	// Checks whether this instance contains no flags.
	bool isEmpty() const;

	bool equals(const TBitFlags& other) const;

	// TODO: clear all flags or clear specific ones

private:
	Value m_bits;

	static Value collectFlags(const FlagsSet& flagsSet);
};

}// end namespace ph

#include "Utility/TBitFlags.ipp"