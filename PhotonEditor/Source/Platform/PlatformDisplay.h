#pragma once

#include <Utility/INoCopyAndMove.h>
#include <Common/primitive_type.h>
#include <Math/TVector2.h>

namespace ph::editor
{

class GHI;

/*! @brief Platform display abstraction.
@exception PlatformException When error occurred and the platform must terminate its operations.
@note Can only be created by `Platform`.
*/
class PlatformDisplay : private INoCopyAndMove
{
	friend class Platform;

protected:
	inline PlatformDisplay() = default;
	inline virtual ~PlatformDisplay() = default;

public:
	virtual GHI* getGHI() const = 0;
	virtual math::Vector2S getSizePx() const = 0;
};

}// end namespace ph::editor
