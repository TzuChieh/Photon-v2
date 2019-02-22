#pragma once

namespace ph
{

namespace constant
{

/*! @brief Value of $\pi$.
*/
template<typename T>
inline constexpr T pi = T(
	3.1415926535897932384626433832795028841971693993751058209749445923078164063);

/*! @brief Value of $1/\pi$.
*/
template<typename T>
inline constexpr T rcp_pi = T(
	0.3183098861837906715377675267450287240689192914809128974953346881177935953);

/*! @brief Value of $2\pi$.
*/
template<typename T>
inline constexpr T two_pi = T(
	6.2831853071795864769252867665590057683943387987502116419498891846156328126);

/*! @brief Value of $1/(2\pi)$.
*/
template<typename T>
inline constexpr T rcp_two_pi = T(
	0.1591549430918953357688837633725143620344596457404564487476673440588967976);

/*! @brief Value of $4\pi$.
*/
template<typename T>
inline constexpr T four_pi = T(
	12.566370614359172953850573533118011536788677597500423283899778369231265625);

/*! @brief Value of $1/(4\pi)$.
*/
template<typename T>
inline constexpr T rcp_four_pi = T(
	0.0795774715459476678844418816862571810172298228702282243738336720294483988);

/*! @brief Value of $\sqrt{2}$.
*/
template<typename T>
inline constexpr T sqrt_2 = T(
	1.4142135623730950488016887242096980785696718753769480731766797379907324785);

/*! @brief Value of $1/\sqrt{2}$.
*/
template<typename T>
inline constexpr T rcp_sqrt_2 = T(
	0.7071067811865475244008443621048490392848359376884740365883398689953662392);

/*! @brief Index type that stores/represents axes.
*/
using AxisIndexType = int;

/*! @name Axis Indices

Standard axis indices used in the renderer.
*/
///@{
inline constexpr AxisIndexType UNKNOWN_AXIS = -1;
inline constexpr AxisIndexType X_AXIS       = 0;
inline constexpr AxisIndexType Y_AXIS       = 1;
inline constexpr AxisIndexType Z_AXIS       = 2;
///@}

}// end namespace constant

}// end namespace ph