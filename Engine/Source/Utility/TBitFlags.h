#pragma once

#include "Utility/traits.h"

#include <type_traits>
#include <initializer_list>

namespace ph
{

/*! @brief Manipulate a value type where each bit is a binary flag.
@tparam Value The underlying value type of this class. Must be an integral.
@tparam Input The input type for the manipulation of flags. Can be integral, scoped/unscoped enum.
*/
template<typename Value, typename Input = Value>
class TBitFlags final
{
	static_assert(std::is_integral_v<Value>);
	static_assert(std::is_integral_v<Input> || std::is_enum_v<Input>);

	static_assert(sizeof(Value) >= sizeof(Input),
		"Input type may overflow Value type.");

private:
	using FlagsSet = std::initializer_list<Input>;

public:
	/*! @brief Creates an instance with no flags.
	*/
	TBitFlags();

	/*! @brief Create with initial flags set.
	*/
	explicit TBitFlags(const FlagsSet& flagsSet);

	/*! @brief Copy construct flags from another `TBitFlags` with a different `Input` type.
	*/
	template<typename OtherInput>
	explicit TBitFlags(const TBitFlags<Value, OtherInput>& otherFlags);

	/*! @brief Unions specified flags into this instance.
	*/
	///@{
	TBitFlags& unionWith(const FlagsSet& flagsSet);
	TBitFlags& unionWith(const TBitFlags& flags);
	///@}

	/*! @brief Intersects this instance with the specified flags.
	*/
	TBitFlags& intersectWith(const FlagsSet& flagsSet);
	
	/*! @brief Enable/disable specified flags.
	*/
	///@{
	TBitFlags& turnOn(const FlagsSet& flagsSet);
	TBitFlags& turnOff(const FlagsSet& flagsSet);
	///@}

	/*! @brief Checks whether this instance contains no specified flags.
	*/
	bool hasNone(const FlagsSet& flagsSet) const;

	/*! @brief Checks whether this instance contains at least one of the specified flags.
	*/
	bool hasAny(const FlagsSet& flagsSet) const;

	/*! @brief Checks whether this instance contains all of the specified flags.
	*/
	bool hasAll(const FlagsSet& flagsSet) const;

	/*! @brief Checks whether this instance contains exactly the specified flags. No more, no less.
	*/
	bool hasExactly(const FlagsSet& flagsSet) const;
	
	/*! @brief Checks whether this instance contains no flags.
	*/
	bool isEmpty() const;

	bool isEqual(const TBitFlags& other) const;

	/*! @brief Get the value representing current flags.
	*/
	Value get() const;

	// TODO: method for clear all flags
	TBitFlags& set(const FlagsSet& flagsSet);

	/*! @brief Get the enum representing current flags.
	This method is only defined for enum flags.
	*/
	inline Input getEnum() const 
		requires CIsEnum<Input> && (sizeof(Input) >= sizeof(Value))
	{
		return static_cast<Input>(m_bits);
	}

private:
	Value m_bits;

	static Value collectFlags(const FlagsSet& flagsSet);
};

/*! @brief Convenient type for using scoped/unscoped enum for bit flags.
*/
template<CIsEnum EnumType>
using TEnumFlags = TBitFlags<std::underlying_type_t<EnumType>, EnumType>;

}// end namespace ph

#define PH_DEFINE_INLINE_ENUM_FLAG_OPERATORS(EnumType)\
	static_assert(::ph::CIsEnum<EnumType>, #EnumType " must be an enum type");\
	\
	inline EnumType operator | (const EnumType lhs, const EnumType rhs)\
	{\
		return ::ph::TEnumFlags<EnumType>({lhs, rhs}).getEnum();\
	}\
	\
	inline EnumType operator & (const EnumType lhs, const EnumType rhs)\
	{\
		return ::ph::TEnumFlags<EnumType>({lhs}).intersectWith({rhs}).getEnum();\
	}

#include "Utility/TBitFlags.ipp"
