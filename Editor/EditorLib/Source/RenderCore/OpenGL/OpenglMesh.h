#pragma once

#include "RenderCore/GHIMesh.h"
#include "ThirdParty/glad2.h"

#include <Utility/TSpan.h>

namespace ph::editor
{

class GHIStorage;

class OpenglMesh : public GHIMesh
{
public:
	OpenglMesh(
		const GHIInfoMeshVertexLayout& layout,
		TSpanView<std::shared_ptr<GHIVertexStorage>> vertexStorages);

	OpenglMesh(
		const GHIInfoMeshVertexLayout& layout,
		TSpanView<std::shared_ptr<GHIVertexStorage>> vertexStorages,
		const std::shared_ptr<GHIIndexStorage>& indexStorage);

	~OpenglMesh() override;

	void bind() override;

private:
	static GLuint getOpenglHandle(GHIStorage& storage);

	GLuint m_vaoID;
};

}// end namespace ph::editor
