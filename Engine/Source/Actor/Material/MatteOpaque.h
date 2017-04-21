#pragma once

#include "Actor/Material/Material.h"
#include "Core/SurfaceBehavior/BSDF/LambertianDiffuse.h"
#include "Math/TVector3.h"
#include "Actor/Texture/Texture.h"
#include "FileIO/SDL/TCommandInterface.h"

#include <memory>

namespace ph
{

class MatteOpaque : public Material, public TCommandInterface<MatteOpaque>
{
public:
	MatteOpaque();
	virtual ~MatteOpaque() override;

	virtual void populateSurfaceBehavior(SurfaceBehavior* const out_surfaceBehavior) const override;

	void setAlbedo(const Vector3R& albedo);
	void setAlbedo(const real r, const real g, const real b);
	void setAlbedo(const std::shared_ptr<Texture>& albedo);

private:
	LambertianDiffuse m_bsdf;

// command interface
public:
	MatteOpaque(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static ExitStatus ciExecute(const std::shared_ptr<MatteOpaque>& targetResource, const std::string& functionName, const InputPacket& packet);
};

}// end namespace ph