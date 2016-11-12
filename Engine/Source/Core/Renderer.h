#pragma once

namespace ph
{

class World;
class Camera;
class Frame;

class Renderer
{
public:
	virtual ~Renderer() = 0;

	virtual void render(const World& world, Camera& camera, Frame* const out_frame) const = 0;
};

}// end namespace ph