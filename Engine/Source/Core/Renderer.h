#pragma once

namespace ph
{

class World;
class Camera;
class HDRFrame;

class Renderer
{
public:
	virtual ~Renderer() = 0;

	virtual void render(const World& world, const Camera& camera, HDRFrame* const out_frame) const = 0;
};

}// end namespace ph