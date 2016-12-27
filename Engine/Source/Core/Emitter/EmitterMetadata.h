#pragma once

namespace ph
{

class Material;
class Transform;
class SurfaceBehavior;

class EmitterMetadata final
{
public:
	const Transform*     m_localToWorld;
	const Transform*     m_worldToLocal;

	EmitterMetadata();
};

}// end namespace ph