#pragma once

#include <vector>

namespace ph::editor
{

class RenderThreadUpdateContext;
class GHIThreadCaller;
class CustomRenderContent;

class RTRScene final
{
public:
	~RTRScene();

	void addCustomRenderContent(CustomRenderContent* content);
	void removeCustomRenderContent(CustomRenderContent* content);

private:
	friend class RenderThread;

	void update(const RenderThreadUpdateContext& ctx);
	void createGHICommands(GHIThreadCaller& caller);

private:
	std::vector<CustomRenderContent*> m_customRenderContents;
};

}// end namespace ph::editor
