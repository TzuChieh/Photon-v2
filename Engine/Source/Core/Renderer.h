#pragma once

#include "Common/primitive_type.h"

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
	virtual float32 queryPercentageProgress() const = 0;

	inline bool isReady() const
	{
		if(!m_sampleGenerator)
		{
			std::cerr << "warning: renderer's sample generator is unset" << std::endl;
			return false;
		}

		return true;
	}

	inline void setSampleGenerator(SampleGenerator* const sampleGenerator)
	{
		if(!sampleGenerator)
		{
			std::cerr << "warning: at Renderer::setSampleGenerator(), input is null" << std::endl;
		}

		m_sampleGenerator = sampleGenerator;
	}

protected:
	SampleGenerator* m_sampleGenerator;
};

}// end namespace ph