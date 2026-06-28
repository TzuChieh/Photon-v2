#pragma once

#include "Engine/Utility/traits.h"

#include <Common/compiler.h>

#include <type_traits>
#include <initializer_list>

namespace ph
{

/*! @brief Manipulate a value type where each bit is a binary flag.
@tparam Value The underlying value type of this class. Must be an integral.
@tparam Input The input type for the manipulation of flags. Can be integral, scoped/unscoped enum.
The conversion from `Input` to `Value` is done via `static_cast` if unspecified.
*/
template<typename Value, typename Input = Value>
class TBitFlags final
{
	static_assert(std::is_integral_v<Value>);
	static_assert(std::is_integral_v<Input> || std::is_enum_v<Input>);

	// We store the flags in an explicitly specified integral type,
	// so enums without fixed underlying type can be used.
	static_assert(sizeof(Value) >= sizeof(Input),
		"Input type may overflow Value type.");

private:
	using FlagsSet = std::initializer_list<Input>;

public:
	/*! @brief Creates an instance with no flags.
	*/
	constexpr TBitFlags();

	/*! @brief Create with initial flags set.
	*/
	constexpr TBitFlags(const FlagsSet& flagsSet);

	/*! @brief Create with initial flags set.
	The conversion from `Input` to `Value` is done via `static_cast`.
	*/
	constexpr TBitFlags(Input flagsSet);

	/*! @brief Copy construct flags from another `TBitFlags` with a different `Input` type.
	Effectively copies the underlying value directly.
	*/
	template<typename OtherInput>
	constexpr TBitFlags(const TBitFlags<Value, OtherInput>& otherFlags);

	/*! @brief Unions specified flags into this instance.
	*/
	///@{
	constexpr TBitFlags& unionWith(const FlagsSet& flagsSet);
	constexpr TBitFlags& unionWith(const TBitFlags& flags);
	///@}

	/*! @brief Intersects this instance with the specified flags.
	*/
	///@{
	constexpr TBitFlags& intersectWith(const FlagsSet& flagsSet);
	constexpr TBitFlags& intersectWith(const TBitFlags& flags);
	///@}
	
	/*! @brief Enable/disable specified flags.
	*/
	///@{
	constexpr TBitFlags& turnOn(const FlagsSet& flagsSet);
	constexpr TBitFlags& turnOn(const TBitFlags& flags);
	constexpr TBitFlags& turnOff(const FlagsSet& flagsSet);
	constexpr TBitFlags& turnOff(const TBitFlags& flags);
	///@}

	/*! @brief Checks whether this instance contains no specified flags.
	*/
	///@{
	constexpr bool hasNone(const FlagsSet& flagsSet) const;
	constexpr bool hasNone(Input flagsSet) const;
	///@}

	/*! @brief Checks whether this instance contains at least one of the specified flags.
	*/
	///@{
	constexpr bool hasAny(const FlagsSet& flagsSet) const;
	constexpr bool hasAny(Input flagsSet) const;
	///@}

	/*! @brief Checks whether this instance contains all of the specified flags.
	*/
	///@{
	constexpr bool hasAll(const FlagsSet& flagsSet) const;
	constexpr bool hasAll(Input flagsSet) const;
	///@}

	/*! @brief Checks whether this instance contains exactly the specified flags. No more, no less.
	*/
	///@{
	constexpr bool hasExactly(const FlagsSet& flagsSet) const;
	constexpr bool hasExactly(Input flagsSet) const;
	///@}

	/*! @brief Checks whether this single flag is fully contained.
	@param singleFlag The flag to test. If the flag combines multiple flags, this method is
	effectively the same as `hasAll()`.
	*/
	constexpr bool has(Input singleFlag) const;

	/*! @brief Checks whether this single flag is fully absent.
	@param singleFlag The flag to test. If the flag combines multiple flags, this method is
	effectively the same as `hasNone()`.
	*/
	constexpr bool hasNo(Input singleFlag) const;
	
	/*! @brief Checks whether this instance contains no flags.
	*/
	constexpr bool isEmpty() const;

	constexpr bool isEqual(const TBitFlags& other) const;

	/*! @brief Get the value representing current flags.
	*/
	constexpr Value get() const;

	/*! @brief Clear all flags.
	*/
	constexpr TBitFlags& clear();

	constexpr TBitFlags& set(const FlagsSet& flagsSet);
	constexpr TBitFlags& set(Input flagsSet);

	/*! @brief Get the enum representing current flags.
	This method is only defined for enum flags.
	*/
	constexpr Input getEnum() const
	requires CEnum<Input> && (sizeof(Input) >= sizeof(Value))
	{
		return static_cast<Input>(m_bits);
	}

	constexpr bool operator == (const TBitFlags& rhs) const;

#if !PH_COMPILER_HAS_P2468R2
	constexpr bool operator != (const TBitFlags& rhs) const;
#endif

private:
	Value m_bits;

	static constexpr Value collectFlags(const FlagsSet& flagsSet);
};

/*! @brief Convenient type for using scoped/unscoped enum for bit flags.
*/
template<CEnum EnumType>
using TEnumFlags = TBitFlags<std::underlying_type_t<EnumType>, EnumType>;

}// end namespace ph

/*! @brief Defines `operator |` and `operator &` for `EnumType`.
*/
#define PH_DEFINE_INLINE_ENUM_FLAG_OPERATORS(EnumType)\
	static_assert(::ph::CEnum<EnumType>, #EnumType " must be an enum type");\
	\
	inline constexpr EnumType operator | (const EnumType lhs, const EnumType rhs)\
	{\
		return ::ph::TEnumFlags<EnumType>({lhs, rhs}).getEnum();\
	}\
	\
	inline constexpr EnumType operator & (const EnumType lhs, const EnumType rhs)\
	{\
		return ::ph::TEnumFlags<EnumType>({lhs}).intersectWith({rhs}).getEnum();\
	}

#include "Engine/Utility/TBitFlags.ipp"
