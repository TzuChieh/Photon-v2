#pragma once

namespace ph::editor
{

class DesignerScene;
class RenderThreadCaller;

class DesignerObject
{
public:
	virtual ~DesignerObject();

	virtual void onInitialize(DesignerScene& scene);
	virtual void onUninitialize(DesignerScene& scene);
	virtual void onInitialize(RenderThreadCaller& caller);
	virtual void onUninitialize(RenderThreadCaller& caller);

	void setScene(DesignerScene* scene);
	DesignerScene& getScene();
	const DesignerScene& getScene() const;

private:
	DesignerScene* m_scene;
};

}// end namespace ph::editor
