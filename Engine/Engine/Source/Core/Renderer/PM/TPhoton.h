#pragma once

#include "Utility/utility.h"
#include "Utility/traits.h"
#include "Math/TVector3.h"
#include "Core/LTA/SidednessAgreement.h"

#include <utility>
#include <type_traits>

namespace ph
{

enum class EPhotonData
{
	ThroughputRadiance,
	Pos,
	FromDir,
	GeometryNormal,
	PathLength
};

template<typename T>
concept CPhoton = std::is_trivially_copyable_v<T> && requires
{
	typename T::PMPhotonTag;
};

template<typename Derived>
class TPhoton
{
public:
	using PMPhotonTag = void;

	template<EPhotonData TYPE>
	static constexpr bool has();

	template<EPhotonData TYPE>
	decltype(auto) get() const;

	template<EPhotonData TYPE, typename T>
	void set(const T& value);

// Hide special members as this class is not intended to be used polymorphically.
// It is derived class's choice to expose them (by defining them in public) or not.
protected:
	PH_DEFINE_INLINE_RULE_OF_5_MEMBERS(TPhoton);
};

template<typename Derived>
template<EPhotonData TYPE>
inline constexpr bool TPhoton<Derived>::has()
{
	static_assert(requires
		{
			{ Derived::template impl_has<TYPE>() } -> CSame<bool>;
		},
		"A photon mapping photon type must implement a static method callable as "
		"`impl_has<EPhotonData{}>() -> bool`.");

	return Derived::template impl_has<TYPE>();
}

template<typename Derived>
template<EPhotonData TYPE>
inline decltype(auto) TPhoton<Derived>::get() const
{
	static_assert(requires (const Derived derived)
		{
			{ derived.template impl_get<TYPE>() } -> CNotSame<void>;
		},
		"A photon mapping photon type must implement a method callable as "
		"`impl_get<EPhotonData{}>() const -> (any type)`.");

	return static_cast<const Derived&>(*this).template impl_get<TYPE>();
}

template<typename Derived>
template<EPhotonData TYPE, typename T>
inline void TPhoton<Derived>::set(const T& value)
{
	static_assert(requires (Derived derived, T value)
		{
			{ derived.template impl_set<TYPE>(value) } -> CSame<void>;
		},
		"A photon mapping photon type must implement a method callable as "
		"`impl_set<EPhotonData{}, T{}>(T{}) -> void`.");

	static_cast<Derived&>(*this).template impl_set<TYPE>(value);
}

}// end namespace ph
