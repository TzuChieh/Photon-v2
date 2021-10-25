#pragma once

#include "Common/config.h"
#include "Common/primitive_type.h"
#include "Math/TVector2.h"
#include "Math/TVector3.h"
#include "Core/Texture/TTexture.h"

#include <memory>
#include <string>
#include <cstddef>

namespace ph { class Image; }
namespace ph { class ActorCookingContext; }

namespace ph
{

class SwizzableImageResource final
{
public:
	SwizzableImageResource(std::shared_ptr<Image> image, std::string swizzleSubscripts);

	std::shared_ptr<TTexture<real>> genRealTexture(ActorCookingContext& ctx);
	std::shared_ptr<TTexture<math::Vector2R>> genVector2RTexture(ActorCookingContext& ctx);
	std::shared_ptr<TTexture<math::Vector3R>> genVector3RTexture(ActorCookingContext& ctx);
	std::shared_ptr<TTexture<math::Vector4R>> genVector4RTexture(ActorCookingContext& ctx);

private:
	std::shared_ptr<Image> m_image;
	std::string            m_swizzleSubscripts;
};

}// end namespace ph
