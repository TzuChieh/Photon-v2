#pragma once

#include <Math/TMatrix4.h>
#include <Math/TVector3.h>

namespace ph::editor
{

class ProjectiveView final
{
public:
	math::Matrix4R modelToView = math::Matrix4R::IDENTITY();
	math::Matrix4R viewToProjection = math::Matrix4R::IDENTITY();
	math::Vector3R viewPosition = math::Vector3R(0, 0, 0);
};

}// end namespace ph::editor
