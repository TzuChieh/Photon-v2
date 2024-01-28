#include "RenderCore/OpenGL/OpenglMesh.h"
#include "RenderCore/VertexStorage.h"
#include "RenderCore/IndexStorage.h"
#include "RenderCore/OpenGL/opengl_enums.h"

#include <Common/assertion.h>
#include <Utility/utility.h>
#include <Common/logging.h>

namespace ph::editor::ghi
{

OpenglMesh::OpenglMesh(
	const MeshVertexLayoutInfo& layout,
	TSpanView<std::shared_ptr<VertexStorage>> vertexStorages)
	
	: OpenglMesh(layout, vertexStorages, nullptr)
{}

OpenglMesh::OpenglMesh(
	const MeshVertexLayoutInfo& layout,
	TSpanView<std::shared_ptr<VertexStorage>> vertexStorages,
	const std::shared_ptr<IndexStorage>& indexStorage)

	: Mesh(layout, vertexStorages, indexStorage)

	, m_vaoID(0)
{
	glCreateVertexArrays(1, &m_vaoID);

	// Each attribute use an independent VBO binding point (may be wasteful, but simplifies the logic)
	for(std::size_t attrIdx = 0; attrIdx < layout.numAttributes; ++attrIdx)
	{
		const auto& binding = layout.attributeBindings[attrIdx];
		VertexStorage& storage = getVertexStorage(binding.storageIndex);
		const VertexAttributeLocatorInfo& attrLocator = storage.getFormat().attributes[binding.attributeIndex];

		const auto attributeIdx = lossless_cast<GLuint>(attrIdx);
		const auto vboBindingPointIdx = lossless_cast<GLuint>(attrIdx);

		// Use an independent VBO binding point (though this could be shared among multiple attributes
		// for some attribute/struct arrangements)
		glVertexArrayVertexBuffer(
			m_vaoID,
			vboBindingPointIdx,
			getOpenglHandle(storage),
			lossless_cast<GLintptr>(attrLocator.strideOffset),
			lossless_cast<GLsizei>(attrLocator.strideSize));

		glEnableVertexArrayAttrib(m_vaoID, attributeIdx);

		glVertexArrayAttribFormat(
			m_vaoID,
			attributeIdx,
			lossless_cast<GLuint>(attrLocator.numElements),
			opengl::to_data_type(attrLocator.elementType),
			attrLocator.shouldNormalize ? GL_TRUE : GL_FALSE,
			0);

		glVertexArrayAttribBinding(m_vaoID, attributeIdx, vboBindingPointIdx);
	}

	// Potentially bind an IBO
	if(hasIndexStorage())
	{
		IndexStorage& indexStorage = getIndexStorage();

		// For drawing purposes, OpenGL supports only these types
		if(!(indexStorage.getIndexType() == EStorageElement::UInt8 ||
		     indexStorage.getIndexType() == EStorageElement::UInt16 ||
		     indexStorage.getIndexType() == EStorageElement::UInt32))
		{
			PH_DEFAULT_LOG(Error,
				"[OpenglMesh] using index storage with unsupported index type for drawing");
		}

		glVertexArrayElementBuffer(m_vaoID, getOpenglHandle(getIndexStorage()));
	}
}

OpenglMesh::~OpenglMesh()
{
	glDeleteVertexArrays(1, &m_vaoID);
}

void OpenglMesh::bind()
{
	PH_ASSERT_NE(m_vaoID, 0);

	glBindVertexArray(m_vaoID);
}

GLuint OpenglMesh::getOpenglHandle(Storage& storage)
{
	const auto nativeHandle = storage.getNativeHandle();

	PH_ASSERT(std::holds_alternative<uint64>(nativeHandle));
	return lossless_cast<GLuint>(std::get<uint64>(nativeHandle));
}

}// end namespace ph::editor::ghi
