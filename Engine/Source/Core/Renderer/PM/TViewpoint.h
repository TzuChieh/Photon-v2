#pragma once

namespace ph
{

enum class EViewpointData
{
	SURFACE_HIT,
	FILM_NDC,
	RADIUS,
	NUM_PHOTONS,
	TAU,
	VIEW_THROUGHPUT,
	VIEW_DIR,
	VIEW_RADIANCE
};

template<typename Derived>
class TViewpoint
{
	friend Derived;

public:
	template<EViewpointData TYPE>
	static constexpr bool has();

	template<EViewpointData TYPE>
	decltype(auto) get() const;

	template<EViewpointData TYPE, typename T>
	void set(const T& value);

private:
	TViewpoint() = default;
};

// In-header Implementations:

template<typename Derived>
template<EViewpointData TYPE>
inline constexpr bool TViewpoint<Derived>::has()
{
	return Derived::template impl_has<TYPE>();
}

template<typename Derived>
template<EViewpointData TYPE>
inline decltype(auto) TViewpoint<Derived>::get() const
{
	return static_cast<const Derived&>(*this).template impl_get<TYPE>();
}

template<typename Derived>
template<EViewpointData TYPE, typename T>
inline void TViewpoint<Derived>::set(const T& value)
{
	static_cast<Derived&>(*this).template impl_set<TYPE>(value);
}

}// end namespace ph