#pragma once

#include <Math/TVector2.h>
#include <Common/primitive_type.h>

namespace ph::editor
{

class Display
{
public:
	inline virtual ~Display() = default;

	virtual void present() = 0;
	virtual math::Vector2S getSizePx() const = 0;
};

}// end namespace ph::editor
