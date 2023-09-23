#pragma once

#include "Render/ISceneResource.h"

namespace ph::editor { class RenderThreadUpdateContext; }

namespace ph::editor::render
{

enum class EProcessOrder
{
	PreProcess = 0,
	ViewContent,
	ViewRender,
	PostProcess,
	Overlay
};

class IDynamicSceneResource : public ISceneResource
{
public:
	IDynamicSceneResource();
	~IDynamicSceneResource() override;

	virtual void update(const RenderThreadUpdateContext& ctx) = 0;
	virtual void createGHICommands(GHIThreadCaller& caller) = 0;
	virtual EProcessOrder getProcessOrder() const;

	void setupGHI(GHIThreadCaller& caller) override = 0;
	void cleanupGHI(GHIThreadCaller& caller) override = 0;
	bool isDynamic() const override;
};

inline bool IDynamicSceneResource::isDynamic() const
{
	return true;
}

inline EProcessOrder IDynamicSceneResource::getProcessOrder() const
{
	return EProcessOrder::ViewContent;
}

}// end namespace ph::editor::render
