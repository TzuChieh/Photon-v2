#pragma once

#include "RenderCore/EGraphicsAPI.h"
#include "ThirdParty/GLFW3_fwd.h"

#include <Utility/INoCopyAndMove.h>
#include <Common/primitive_type.h>
#include <Math/TVector2.h>

#include <variant>

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
	PlatformDisplay();
	virtual ~PlatformDisplay();

public:
	virtual GHI* getGHI() const = 0;
	virtual math::Vector2S getSizePx() const = 0;
	virtual math::Vector2S getFramebufferSizePx() const = 0;
	virtual EGraphicsAPI getGraphicsApiType() const = 0;

	virtual float32 getDpiScale() const;

	using NativeWindow = std::variant<
		std::monostate,
		GLFWwindow*>;

	virtual NativeWindow getNativeWindow() const;
};

inline float32 PlatformDisplay::getDpiScale() const
{
	return 1.0f;
}

}// end namespace ph::editor
