#include "EditorLib/Designer/TextualNoteObject.h"

#include <Engine/Math/Transform/TDecomposedTransform.h>

namespace ph::editor
{

math::TDecomposedTransform<real> TextualNoteObject::getLocalToParent() const
{
	return m_textTransform.getDecomposed();
}

void TextualNoteObject::setLocalToParent(const math::TDecomposedTransform<real>& transform)
{
	m_textTransform.set(transform);
}

}// end namespace ph::editor
