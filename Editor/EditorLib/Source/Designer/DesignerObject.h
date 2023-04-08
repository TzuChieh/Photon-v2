#pragma once

namespace ph::editor
{

class DesignerScene;
class RenderThreadCaller;

class DesignerObject
{
public:
	virtual ~DesignerObject();

	virtual void onInit(DesignerScene& scene);
	virtual void onUninit(DesignerScene& scene);
	virtual void onRenderInit(RenderThreadCaller& caller);
	virtual void onRenderUninit(RenderThreadCaller& caller);

	void setScene(DesignerScene* scene);
	DesignerScene& getScene();
	const DesignerScene& getScene() const;

private:
	DesignerScene* m_scene;
};

}// end namespace ph::editor
