#pragma once

#include "SDL/TSdlResourceBase.h"
#include "Common/primitive_type.h"
#include "Math/Transform/TDecomposedTransform.h"
#include "Math/Transform/Transform.h"

#include <memory>

namespace ph
{

class Time;
class CookingContext;
class CookedMotion;
class MotionCookConfig;

class MotionSource : public TSdlResourceBase<ESdlTypeCategory::Ref_Motion>
{
public:
	/*! @brief Store data suitable for rendering into `out_motion`.
	*/
	virtual void storeCooked(
		CookedMotion& out_motion,
		const CookingContext& ctx,
		const MotionCookConfig& config) const = 0;
	
	// DEPRECATED
	virtual std::unique_ptr<math::Transform> genLocalToWorld(
		const Time& start, 
		const Time& end) const = 0;

	/*! @brief Create a `CookedMotion` that contains data suitable for rendering.
	*/
	CookedMotion* createCooked(
		const CookingContext& ctx,
		const MotionCookConfig& config) const;
};

}// end namespace ph
