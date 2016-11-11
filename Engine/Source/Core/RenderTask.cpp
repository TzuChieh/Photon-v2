#include "Core/RenderTask.h"
#include "Core/Renderer.h"
#include "Image/Frame.h"
#include "World/World.h"
#include "Camera/Camera.h"

namespace ph
{

RenderTask::RenderTask(World* const world, Camera* const camera, Renderer* const renderer, Frame* const frame) :
	m_world(world), m_camera(camera), m_renderer(renderer), m_frame(frame)
{

}

void RenderTask::run() const
{
	m_renderer->render(*m_world, *m_camera, m_frame);
}

}// end namespace ph