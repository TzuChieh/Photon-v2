#pragma once

namespace ph::editor
{

class GHIThreadCaller;

class RTRResource
{
public:
	virtual ~RTRResource();

	virtual void createGHIResource(GHIThreadCaller& caller) = 0;
	virtual void destroyGHIResource(GHIThreadCaller& caller) = 0;
};

}// end namespace ph::editor
