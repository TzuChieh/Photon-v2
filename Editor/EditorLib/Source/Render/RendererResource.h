#pragma once

namespace ph::editor
{

class GHIThreadCaller;

class RendererResource
{
public:
	virtual ~RendererResource();

	virtual void setupGHI(GHIThreadCaller& caller) = 0;
	virtual void cleanupGHI(GHIThreadCaller& caller) = 0;
};

}// end namespace ph::editor
