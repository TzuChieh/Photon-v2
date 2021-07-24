#pragma once

#include "Actor/LightSource/LightSource.h"
#include "Math/math_fwd.h"
#include "DataIO/FileSystem/Path.h"

#include <memory>
#include <string>

namespace ph
{

class Image;

class ModelSource : public LightSource
{
public:
	explicit ModelSource(const math::Vector3R& emittedRgbRadiance);
	explicit ModelSource(const Path& imagePath);
	explicit ModelSource(const std::shared_ptr<Image>& emittedRadiance);

	std::unique_ptr<Emitter> genEmitter(
		ActorCookingContext& ctx, EmitterBuildingMaterial&& data) const override;

	std::shared_ptr<Geometry> genGeometry(ActorCookingContext& ctx) const override;

	std::shared_ptr<Material> genMaterial(ActorCookingContext& ctx) const override;

	void setGeometry(const std::shared_ptr<Geometry>& geometry);
	void setMaterial(const std::shared_ptr<Material>& material);
	void setBackFaceEmit(bool isBackFaceEmit);

private:
	std::shared_ptr<Image>    m_emittedRadiance;
	std::shared_ptr<Geometry> m_geometry;
	std::shared_ptr<Material> m_material;
	bool                      m_isBackFaceEmit;
};

}// end namespace ph
