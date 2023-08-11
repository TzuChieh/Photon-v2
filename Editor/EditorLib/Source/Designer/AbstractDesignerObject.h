#pragma once

#include "Designer/designer_fwd.h"

#include <Common/primitive_type.h>
#include <SDL/Object.h>
#include <SDL/sdl_interface.h>
#include <Utility/TBitFlags.h>

namespace ph::editor
{

class UIPropertyLayout;

class AbstractDesignerObject : public Object
{
public:
	AbstractDesignerObject();
	AbstractDesignerObject(const AbstractDesignerObject& other);
	AbstractDesignerObject(AbstractDesignerObject&& other) noexcept;
	~AbstractDesignerObject() override = 0;

	/*! @brief Create custom property layout.
	*/
	virtual UIPropertyLayout layoutProperties();

	AbstractDesignerObject& operator = (const AbstractDesignerObject& rhs);
	AbstractDesignerObject& operator = (AbstractDesignerObject&& rhs) noexcept;

protected:
	const TEnumFlags<EObjectState>& getState() const;

private:
	// For accessing internal members managed by designer scene
	friend class DesignerScene;
	friend class DesignerSceneMetaInfo;

	TEnumFlags<EObjectState> m_state;
	uint64 m_sceneStorageIndex;

	TEnumFlags<EObjectState>& state();
	uint64 getSceneStorageIndex() const;
	void setSceneStorageIndex(uint64 storageIndex);

public:
	PH_DEFINE_SDL_CLASS(TSdlOwnerClass<AbstractDesignerObject>)
	{
		ClassType clazz("abstract-dobj");
		clazz.docName("Abstract Designer Object");
		clazz.description("A common base for designer-related object classes.");
		clazz.baseOn<Object>();
		return clazz;
	}
};

}// end namespace ph::editor

#include "Designer/AbstractDesignerObject.ipp"
