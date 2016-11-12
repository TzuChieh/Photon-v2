#pragma once

#include <iostream>

namespace ph
{

class World;
class Camera;
class Frame;
class SampleGenerator;
class Film;

class Renderer
{
public:
	Renderer();
	virtual ~Renderer() = 0;

	virtual void render(const World& world, const Camera& camera) const = 0;

	inline void setSampleGenerator(SampleGenerator* const sampleGenerator)
	{
		if(sampleGenerator == nullptr)
		{
			std::cerr << "warning: at Renderer::setSampleGenerator(), input is null" << std::endl;
		}

		m_sampleGenerator = sampleGenerator;
	}

protected:
	SampleGenerator* m_sampleGenerator;
};

}// end namespace ph