#pragma once

#include "EditorLib/RenderCore/Mesh.h"

#include "EditorLib/ThirdParty/glad2.h"

#include <Engine/Utility/TSpan.h>

namespace ph::editor::ghi
{

class Storage;

class OpenglMesh : public Mesh
{
public:
	OpenglMesh(
		const MeshVertexLayoutInfo& layout,
		TSpanView<std::shared_ptr<VertexStorage>> vertexStorages);

	OpenglMesh(
		const MeshVertexLayoutInfo& layout,
		TSpanView<std::shared_ptr<VertexStorage>> vertexStorages,
		const std::shared_ptr<IndexStorage>& indexStorage);

	~OpenglMesh() override;

	void bind() override;

private:
	static GLuint getOpenglHandle(Storage& storage);

	GLuint m_vaoID;
};

}// end namespace ph::editor::ghi
