#pragma once

#include "Utility/utility.h"
#include "Utility/traits.h"

#include <utility>
#include <type_traits>

namespace ph
{

enum class EViewpointData
{
	SurfaceHit,
	RasterCoord,
	Radius,
	NumPhotons,
	Tau,
	ViewThroughput,
	ViewDir,
	ViewRadiance
};

template<typename T>
concept CViewpoint = std::is_trivially_copyable_v<T> && requires
{
	typename T::PMViewpointTag;
};

template<typename Derived>
class TViewpoint
{
public:
	using PMViewpointTag = void;

	template<EViewpointData TYPE>
	static constexpr bool has();

	template<EViewpointData TYPE>
	decltype(auto) get() const;

	template<EViewpointData TYPE, typename T>
	void set(const T& value);

// Hide special members as this class is not intended to be used polymorphically.
// It is derived class's choice to expose them (by defining them in public) or not.
protected:
	PH_DEFINE_INLINE_RULE_OF_5_MEMBERS(TViewpoint);
};

// In-header Implementations:

template<typename Derived>
template<EViewpointData TYPE>
inline constexpr bool TViewpoint<Derived>::has()
{
	static_assert(requires
		{
			{ Derived::template impl_has<TYPE>() } -> CSame<bool>;
		},
		"A photon mapping viewpoint type must implement static method "
		"`template<EViewpointData TYPE> impl_has() -> bool`.");

	return Derived::template impl_has<TYPE>();
}

template<typename Derived>
template<EViewpointData TYPE>
inline decltype(auto) TViewpoint<Derived>::get() const
{
	static_assert(requires (const Derived derived)
		{
			{ derived.template impl_get<TYPE>() } -> CNotSame<void>;
		},
		"A photon mapping viewpoint type must implement "
		"`template<EViewpointData TYPE> impl_get() const -> (any type)`.");

	return static_cast<const Derived&>(*this).template impl_get<TYPE>();
}

template<typename Derived>
template<EViewpointData TYPE, typename T>
inline void TViewpoint<Derived>::set(const T& value)
{
	static_assert(requires (Derived derived, T value)
		{
			{ derived.template impl_set<TYPE>(value) } -> CSame<void>;
		},
		"A photon mapping viewpoint type must implement "
		"`template<EViewpointData TYPE, typename T> impl_set(const T& value) -> void`.");

	static_cast<Derived&>(*this).template impl_set<TYPE>(value);
}

}// end namespace ph
