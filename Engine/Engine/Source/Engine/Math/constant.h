#pragma once

#include <cstddef>
#include <cstdint>

namespace ph::math
{

namespace constant
{

/*! @brief Value of @f$ \pi @f$.
*/
template<typename T>
inline constexpr T pi = T(
	3.1415926535897932384626433832795028841971693993751058209749445923078164063);

/*! @brief Value of @f$ 1/\pi @f$.
*/
template<typename T>
inline constexpr T rcp_pi = T(
	0.3183098861837906715377675267450287240689192914809128974953346881177935953);

/*! @brief Value of @f$ 2\pi @f$.
*/
template<typename T>
inline constexpr T two_pi = T(
	6.2831853071795864769252867665590057683943387987502116419498891846156328126);

/*! @brief Value of @f$ 1/(2\pi) @f$.
*/
template<typename T>
inline constexpr T rcp_two_pi = T(
	0.1591549430918953357688837633725143620344596457404564487476673440588967976);

/*! @brief Value of @f$ 4\pi @f$.
*/
template<typename T>
inline constexpr T four_pi = T(
	12.566370614359172953850573533118011536788677597500423283899778369231265625);

/*! @brief Value of @f$ 1/(4\pi) @f$.
*/
template<typename T>
inline constexpr T rcp_four_pi = T(
	0.0795774715459476678844418816862571810172298228702282243738336720294483988);

/*! @brief Value of @f$ \pi^2 @f$.
*/
template<typename T>
inline constexpr T pi2 = T(
	9.8696044010893586188344909998761511353136994072407906264133493762200448224);

/*! @brief Value of @f$ \sqrt{2} @f$.
*/
template<typename T>
inline constexpr T sqrt_2 = T(
	1.4142135623730950488016887242096980785696718753769480731766797379907324785);

/*! @brief Value of @f$ 1/\sqrt{2} @f$.
*/
template<typename T>
inline constexpr T rcp_sqrt_2 = T(
	0.7071067811865475244008443621048490392848359376884740365883398689953662392);

/*! @brief Planck's constant (SI units).
*/
template<typename T>
inline constexpr T h_Planck = T(6.62607015e-34);

/*! @brief Light speed in vacuum (SI units).
*/
template<typename T>
inline constexpr T c_light = T(299792458);

template<typename T>
inline constexpr T k_Boltzmann = T(1.380649e-23);

inline constexpr std::size_t KiB = 1024;
inline constexpr std::size_t MiB = KiB * 1024;
inline constexpr std::size_t GiB = MiB * 1024;
inline constexpr std::size_t TiB = GiB * 1024;
inline constexpr std::size_t PiB = TiB * 1024;

/*! @name Axis Indices
Standard axis indices used in the renderer.
*/
///@{
inline constexpr std::size_t UNKNOWN_AXIS = static_cast<std::size_t>(-1);
inline constexpr std::size_t X_AXIS       = 0;
inline constexpr std::size_t Y_AXIS       = 1;
inline constexpr std::size_t Z_AXIS       = 2;
///@}

}// end namespace constant

}// end namespace ph::math
