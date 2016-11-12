#include "Core/RenderTask.h"
#include "Core/Renderer.h"
#include "Image/Frame.h"
#include "World/World.h"
#include "Camera/Camera.h"

#include <iostream>

namespace ph
{

RenderTask::RenderTask(Renderer* const renderer, World* const world, Camera* const camera) :
	m_renderer(renderer), m_world(world), m_camera(camera)
{

}

void RenderTask::run() const
{
	if(isResourceGood())
	{
		m_renderer->render(*m_world, *m_camera);
	}
}

bool RenderTask::isResourceGood() const
{
	bool isGood = true;

	if(m_world == nullptr)
	{
		std::cerr << "World is null" << std::endl;
		isGood = false;
	}

	if(m_camera == nullptr)
	{
		std::cerr << "Camera is null" << std::endl;
		isGood = false;
	}

	if(m_renderer == nullptr)
	{
		std::cerr << "Renderer is null" << std::endl;
		isGood = false;
	}

	return isGood;
}

}// end namespace ph