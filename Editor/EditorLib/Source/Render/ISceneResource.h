#pragma once

namespace ph::editor { class GHIThreadCaller; }

namespace ph::editor::render
{

class ISceneResource
{
public:
	ISceneResource();
	virtual ~ISceneResource();

	virtual void setupGHI(GHIThreadCaller& caller) = 0;
	virtual void cleanupGHI(GHIThreadCaller& caller) = 0;

	virtual bool isDynamic() const;
};

inline bool ISceneResource::isDynamic() const
{
	return false;
}

}// end namespace ph::editor::render
