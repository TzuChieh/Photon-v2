#pragma once

#include "Core/Renderer/RenderWork.h"

namespace ph
{

class RadianceEstimateWork : public RenderWork
{
public:
	void doWork() override;

private:
};

}// end namespace ph