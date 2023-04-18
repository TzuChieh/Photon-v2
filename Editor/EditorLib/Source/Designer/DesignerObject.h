#pragma once

#include "Designer/designer_fwd.h"

#include <Common/primitive_type.h>
#include <Common/config.h>
#include <Utility/INoCopyAndMove.h>
#include <Math/math.h>
#include <Utility/TBitFlags.h>

#include <string>
#include <memory>
#include <cstddef>
#include <vector>

namespace ph::editor
{

class RenderThreadCaller;
class MainThreadUpdateContext;
class MainThreadRenderUpdateContext;
class RenderThreadCaller;

enum class EObjectState : uint32f
{
	// Lifetime management
	Initialized = math::flag_bit<uint32f, 0>(),
	RenderInitialized = math::flag_bit<uint32f, 1>(),
	RenderUninitialized = math::flag_bit<uint32f, 2>(),
	Uninitialized = math::flag_bit<uint32f, 3>(),

	// Category
	Root = math::flag_bit<uint32f, 4>(),
	Ticking = math::flag_bit<uint32f, 5>(),
	RenderTicking = math::flag_bit<uint32f, 6>()
};

class DesignerObject : private INoCopyAndMove
{
public:
	explicit DesignerObject(DesignerScene* scene);
	virtual ~DesignerObject();

	virtual void init();
	virtual void uninit();
	virtual void renderInit(RenderThreadCaller& caller);
	virtual void renderUninit(RenderThreadCaller& caller);
	virtual void update(const MainThreadUpdateContext& ctx);
	virtual void renderUpdate(const MainThreadRenderUpdateContext& ctx);
	virtual void createRenderCommands(RenderThreadCaller& caller);

	DesignerObject* getChild(std::size_t childIndex) const;
	std::size_t numChildren() const;
	bool hasChildren() const;

	template<typename ChildType, typename... DeducedArgs>
	ChildType* initNewChild(DeducedArgs&&... args);

	void removeChild(std::size_t childIndex, bool isRecursive = true);
	void setName(std::string name);
	void setTick(bool shouldTick);
	void setRenderTick(bool shouldTick);
	DesignerScene& getScene();
	const DesignerScene& getScene() const;
	const std::string& getName() const;
	bool isRemoved() const;
	const TEnumFlags<EObjectState>& getState() const;

private:
	DesignerScene* m_scene;
	std::vector<DesignerObject*> m_children;
	std::string m_name;
	uint32 m_isRemoved : 1;

private:
	friend class DesignerScene;

	TEnumFlags<EObjectState> m_state;

	TEnumFlags<EObjectState>& getState();
};

}// end namespace ph::editor

#include "Designer/DesignerObject.ipp"
