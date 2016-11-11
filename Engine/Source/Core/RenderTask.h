#pragma once

namespace ph
{

class Renderer;
class Frame;
class World;
class Camera;

class RenderTask final
{
public:
	RenderTask(World* const world, Camera* const camera, Renderer* const renderer, Frame* const frame);

	void run() const;

private:
	World*    m_world;
	Camera*   m_camera;
	Renderer* m_renderer;
	Frame*    m_frame;
};

}// end namespace ph