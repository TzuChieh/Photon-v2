#pragma once

#include "RenderCore/Mesh.h"

#include "ThirdParty/glad2.h"

#include <Utility/TSpan.h>

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
