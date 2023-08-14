#pragma once

#include "Designer/DesignerObject.h"

#include <SDL/sdl_interface.h>
#include <Utility/utility.h>

namespace ph::editor
{

class FlatDesignerObject : public DesignerObject
{
public:
	~FlatDesignerObject() override = 0;

	math::TDecomposedTransform<real> getLocalToParent() const override = 0;
	void setLocalToParent(const math::TDecomposedTransform<real>& transform) override = 0;

	TSpanView<DesignerObject*> getChildren() const override;
	bool canHaveChildren() const override;

protected:
	PH_DECLARE_RULE_OF_5_MEMBERS_NO_DTOR(FlatDesignerObject);

private:
	DesignerObject* addChild(DesignerObject* childObj) override;
	bool removeChild(DesignerObject* childObj) override;

public:
	PH_DEFINE_SDL_CLASS(TSdlOwnerClass<FlatDesignerObject>)
	{
		ClassType clazz("flat-dobj");
		clazz.docName("Flat Designer Object");
		clazz.description("A designer object that does not allow any children.");
		clazz.baseOn<DesignerObject>();
		return clazz;
	}
};

}// end namespace ph::editor
