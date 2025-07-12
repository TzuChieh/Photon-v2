#pragma once

#include <Common/primitive_type.h>

#include "Engine/Math/math.h"

namespace ph
{

class HitInfo;
class HitDetail;
class HitProbe;
class SurfaceHit;
class VolumeHit;

namespace detail
{

using HitReasonIntType = uint8;

inline constexpr auto hr_unknown_bits      = math::flag_bit<HitReasonIntType, 0>();
inline constexpr auto hr_incident_ray_bits = math::flag_bit<HitReasonIntType, 1>();
inline constexpr auto hr_sampled_pos_bits  = math::flag_bit<HitReasonIntType, 2>();
inline constexpr auto hr_sampled_dir_bits  = math::flag_bit<HitReasonIntType, 3>();

}// end namespace detail

}// end namespace ph
