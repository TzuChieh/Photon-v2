#pragma once

#include "Math/TVector2.h"
#include "Common/assertion.h"

namespace ph
{

template<typename T>
class TCoordinate2_i0e1 final
{
public:
	TCoordinate2_i0e1();
	TCoordinate2_i0e1(const TVector2<T>& coordinates);

	const TVector2<T>& getCoordinates() const;

private:
	TVector2<T> m_coordinates;
};

// Implementations:

template<typename T>
TCoordinate2_i0e1<T>::TCoordinate2_i0e1() : 
	TCoordinate2_i0e1(TVector2<T>(0))
{}

template<typename T>
TCoordinate2_i0e1<T>::TCoordinate2_i0e1(const TVector2<T>& coordinates) : 
	m_coordinates(coordinates)
{
	// HACK: upper bounds should be < 1
	PH_ASSERT(0 <= m_coordinates.x && m_coordinates.x <= 1);
	PH_ASSERT(0 <= m_coordinates.y && m_coordinates.y <= 1);
}

template<typename T>
const TVector2<T>& TCoordinate2_i0e1<T>::getCoordinates() const
{
	return m_coordinates;
}

}// end namespace ph