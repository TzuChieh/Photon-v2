#pragma once

#include <Common/primitive_type.h>
#include <Math/math.h>

#include <memory>

namespace ph::editor
{

class AbstractDesignerObject;
class DesignerObject;
class DesignerResource;
class DesignerScene;

enum class EObjectState : uint32f
{
	// Lifetime management
	HasInitialized = math::flag_bit<uint32f, 0>(),
	HasRenderInitialized = math::flag_bit<uint32f, 1>(),
	HasRenderUninitialized = math::flag_bit<uint32f, 2>(),
	HasUninitialized = math::flag_bit<uint32f, 3>(),

	// Category
	Root = math::flag_bit<uint32f, 4>(),
	Ticking = math::flag_bit<uint32f, 5>(),
	RenderTicking = math::flag_bit<uint32f, 6>(),
	Selected = math::flag_bit<uint32f, 7>(),
	Hidden = math::flag_bit<uint32f, 8>()
};

}// end namespace ph::editor
