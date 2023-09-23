#pragma once

#include "RenderCore/EGraphicsAPI.h"

#include "ThirdParty/GLFW3_fwd.h"

#include <Utility/INoCopyAndMove.h>
#include <Common/primitive_type.h>
#include <Math/TVector2.h>

#include <variant>

namespace ph::editor::ghi { class GraphicsContext; }

namespace ph::editor
{

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
	virtual ghi::GraphicsContext* getGraphicsContext() const = 0;
	virtual math::Vector2S getSizePx() const = 0;
	virtual math::Vector2S getFramebufferSizePx() const = 0;
	virtual ghi::EGraphicsAPI getGraphicsApiType() const = 0;

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
