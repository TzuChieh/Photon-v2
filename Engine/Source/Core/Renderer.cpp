#include "Core/Renderer.h"

#include <iostream>

namespace ph
{

Renderer::Renderer(const uint32 numThreads) : 
	m_sampleGenerator(nullptr), m_numThreads(numThreads)
{
	if(numThreads == 0)
	{
		std::cerr << "warning: at Renderer::Renderer(), specified # of threads is 0" << std::endl;
	}
}

Renderer::~Renderer() = default;

}// end namespace ph