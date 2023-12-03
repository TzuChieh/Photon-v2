#pragma once

namespace ph
{

enum class EPhotonData
{
	THROUGHPUT_RADIANCE,
	POSITION,
	FROM_DIR
};

template<typename Derived>
class TPhoton
{
	friend Derived;

public:
	template<EPhotonData TYPE>
	static constexpr bool has();

	template<EPhotonData TYPE>
	decltype(auto) get() const;

	template<EPhotonData TYPE, typename T>
	void set(const T& value);

private:
	TPhoton() = default;
	~TPhoton() = default;
};

// In-header Implementations:

template<typename Derived>
template<EPhotonData TYPE>
inline constexpr bool TPhoton<Derived>::has()
{
	return Derived::template impl_has<TYPE>();
}

template<typename Derived>
template<EPhotonData TYPE>
inline decltype(auto) TPhoton<Derived>::get() const
{
	return static_cast<const Derived&>(*this).template impl_get<TYPE>();
}

template<typename Derived>
template<EPhotonData TYPE, typename T>
inline void TPhoton<Derived>::set(const T& value)
{
	static_cast<Derived&>(*this).template impl_set<TYPE>(value);
}

}// end namespace ph
