#pragma once

namespace ph::editor
{

class GHIThreadCaller;

class RTRResource
{
public:
	virtual ~RTRResource();

	virtual void setupGHI(GHIThreadCaller& caller) = 0;
	virtual void cleanupGHI(GHIThreadCaller& caller) = 0;
};

}// end namespace ph::editor
