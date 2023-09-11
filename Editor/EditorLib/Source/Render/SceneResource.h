#pragma once

namespace ph::editor { class GHIThreadCaller; }

namespace ph::editor::render
{

class SceneResource
{
public:
	virtual ~SceneResource();

	virtual void setupGHI(GHIThreadCaller& caller) = 0;
	virtual void cleanupGHI(GHIThreadCaller& caller) = 0;
};

}// end namespace ph::editor::render
