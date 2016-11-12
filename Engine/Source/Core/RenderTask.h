#pragma once

namespace ph
{

class Renderer;
class World;
class Camera;

class RenderTask final
{
public:
	RenderTask(Renderer* const renderer, World* const world, Camera* const camera);

	void run() const;

private:
	Renderer* m_renderer;
	World*    m_world;
	Camera*   m_camera;

	bool isResourceGood() const;
};

}// end namespace ph