#pragma once

#include "RenderCore/GHIMeshStorage.h"
#include "ThirdParty/glad2.h"

namespace ph::editor
{

class OpenglMeshStorage : public GHIMeshStorage
{
public:
	OpenglMeshStorage(
		const GHIInfoMeshVertexLayout& layout,
		std::span<std::shared_ptr<GHIVertexStorage>> vertexStorages);

	OpenglMeshStorage(
		const GHIInfoMeshVertexLayout& layout,
		std::span<std::shared_ptr<GHIVertexStorage>> vertexStorages,
		const std::shared_ptr<GHIIndexStorage>& indexStorage);

	~OpenglMeshStorage() override;

	void bind() override;

private:
	static GLuint getOpenglHandle(GHIStorage& storage);

	GLuint m_vaoID;
};

}// end namespace ph::editor
