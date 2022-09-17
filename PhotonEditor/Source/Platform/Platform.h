#pragma once

#include <Common/primitive_type.h>

namespace ph::editor
{

class Input;
class Display;

class Platform
{
public:
	Platform();
	virtual ~Platform();

	virtual void update(float64 deltaS) = 0;
	virtual void render() = 0;
	virtual const Input& getInput() const = 0;
	virtual const Display& getDisplay() const = 0;
};

}// end namespace ph::editor
