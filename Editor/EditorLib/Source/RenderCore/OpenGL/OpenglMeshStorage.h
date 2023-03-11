#pragma once

#include "RenderCore/GHIMeshStorage.h"
#include "ThirdParty/glad2.h"

#include <Utility/TSpan.h>

namespace ph::editor
{

class OpenglMeshStorage : public GHIMeshStorage
{
public:
	OpenglMeshStorage(
		const GHIInfoMeshVertexLayout& layout,
		TSpanView<std::shared_ptr<GHIVertexStorage>> vertexStorages);

	OpenglMeshStorage(
		const GHIInfoMeshVertexLayout& layout,
		TSpanView<std::shared_ptr<GHIVertexStorage>> vertexStorages,
		const std::shared_ptr<GHIIndexStorage>& indexStorage);

	~OpenglMeshStorage() override;

	void bind() override;

private:
	static GLuint getOpenglHandle(GHIStorage& storage);

	GLuint m_vaoID;
};

}// end namespace ph::editor
