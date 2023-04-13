#pragma once

#include "Designer/designer_fwd.h"

#include <Utility/TSpan.h>
#include <Common/primitive_type.h>

#include <string>
#include <vector>
#include <memory>
#include <cstddef>

namespace ph::editor
{

class RenderThreadCaller;
class MainThreadUpdateContext;
class MainThreadRenderUpdateContext;
class RenderThreadCaller;

class DesignerObject
{
public:
	DesignerObject();
	virtual ~DesignerObject();

	virtual void onCreate(DesignerScene* scene);
	virtual void onRemove();
	virtual void onRenderInit(RenderThreadCaller& caller);
	virtual void onRenderUninit(RenderThreadCaller& caller);
	virtual void onUpdate(const MainThreadUpdateContext& ctx);
	virtual void onRenderUpdate(const MainThreadRenderUpdateContext& ctx);
	virtual void onCreateRenderCommands(RenderThreadCaller& caller);

	TSpanView<std::shared_ptr<DesignerObject>> getChildren() const;
	const std::shared_ptr<DesignerObject>& getChild(std::size_t childIndex) const;
	std::size_t numChildren() const;

	template<typename ChildType, typename... DeducedArgs>
	const std::shared_ptr<DesignerObject>& createChild(DeducedArgs&&... args);

	void setScene(DesignerScene* scene);
	void setName(std::string name);
	void setTick(bool isTicking);
	void setRenderTick(bool isTicking);

	DesignerScene& getScene();
	const DesignerScene& getScene() const;
	const std::string& getName() const;

private:
	std::vector<std::shared_ptr<DesignerObject>> m_children;
	DesignerScene* m_scene;
	std::string m_name;
	uint32 m_isTicking : 1;
	uint32 m_isRenderTicking : 1;
};

}// end namespace ph::editor

#include "Designer/DesignerObject.ipp"
