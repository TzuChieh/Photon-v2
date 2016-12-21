#pragma once

namespace ph
{

class Material;
class Transform;
class TextureMapper;

class EmitterMetadata final
{
public:
	const Material*      m_material;
	const Transform*     m_localToWorld;
	const Transform*     m_worldToLocal;
	const TextureMapper* m_textureMapper;

	EmitterMetadata();
};

}// end namespace ph