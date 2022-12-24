#include "RenderCore/OpenGL/OpenglMeshStorage.h"
#include "RenderCore/GHIVertexStorage.h"
#include "RenderCore/GHIIndexStorage.h"
#include "RenderCore/OpenGL/opengl_enums.h"

#include <Common/assertion.h>
#include <Utility/utility.h>
#include <Common/logging.h>

namespace ph::editor
{

OpenglMeshStorage::OpenglMeshStorage(
	const GHIInfoMeshVertexLayout& layout,
	std::span<std::shared_ptr<GHIVertexStorage>> vertexStorages)
	
	: OpenglMeshStorage(layout, vertexStorages, nullptr)
{}

OpenglMeshStorage::OpenglMeshStorage(
	const GHIInfoMeshVertexLayout& layout,
	std::span<std::shared_ptr<GHIVertexStorage>> vertexStorages,
	const std::shared_ptr<GHIIndexStorage>& indexStorage)

	: GHIMeshStorage(layout, vertexStorages, indexStorage)

	, m_vaoID(0)
{
	glCreateVertexArrays(1, &m_vaoID);

	// Each attribute use an independent VBO binding point (may be wasteful, but simplifies the logic)
	for(std::size_t attrIdx = 0; attrIdx < layout.numAttributes; ++attrIdx)
	{
		const auto& binding = layout.attributeBindings[attrIdx];
		GHIVertexStorage& storage = getVertexStorage(binding.storageIndex);
		const GHIInfoVertexAttributeLocator& attrLocator = storage.getFormat().attributes[binding.attributeIndex];

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
			opengl::translate(attrLocator.elementType),
			attrLocator.shouldNormalize ? GL_TRUE : GL_FALSE,
			0);

		glVertexArrayAttribBinding(m_vaoID, attributeIdx, vboBindingPointIdx);
	}

	// Potentially bind an IBO
	if(hasIndexStorage())
	{
		GHIIndexStorage& indexStorage = getIndexStorage();

		// For drawing purposes, OpenGL supports only these types
		if(!(indexStorage.getIndexType() == EGHIInfoStorageElement::UInt8 ||
		     indexStorage.getIndexType() == EGHIInfoStorageElement::UInt16 ||
		     indexStorage.getIndexType() == EGHIInfoStorageElement::UInt32))
		{
			PH_DEFAULT_LOG_ERROR(
				"[OpenglMeshStorage] using index storage with unsupported index type for drawing");
		}

		glVertexArrayElementBuffer(m_vaoID, getOpenglHandle(getIndexStorage()));
	}
}

OpenglMeshStorage::~OpenglMeshStorage()
{
	glDeleteVertexArrays(1, &m_vaoID);
}

void OpenglMeshStorage::bind()
{
	PH_ASSERT_NE(m_vaoID, 0);

	glBindVertexArray(m_vaoID);
}

GLuint OpenglMeshStorage::getOpenglHandle(GHIStorage& storage)
{
	const auto nativeHandle = storage.getNativeHandle();

	PH_ASSERT(std::holds_alternative<uint64>(nativeHandle));
	return lossless_cast<GLuint>(std::get<uint64>(nativeHandle));
}

}// end namespace ph::editor
