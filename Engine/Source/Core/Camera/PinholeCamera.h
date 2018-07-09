#pragma once

#include "Core/Camera/Camera.h"
#include "Common/primitive_type.h"
#include "FileIO/SDL/TCommandInterface.h"
#include "Core/Camera/PerspectiveCamera.h"

namespace ph
{

class PinholeCamera final : public PerspectiveCamera, public TCommandInterface<PinholeCamera>
{
public:
	virtual ~PinholeCamera() override;

	void genSensedRay(const Vector2R& filmNdcPos, Ray* out_ray) const override;
	virtual void evalEmittedImportanceAndPdfW(const Vector3R& targetPos, Vector2R* const out_filmCoord, Vector3R* const out_importance, real* out_filmArea, real* const out_pdfW) const override;

private:
	inline const Vector3R& getPinholePos() const
	{
		return getPosition();
	}

	Vector3R genSensedRayDir(const Vector2R& filmNdcPos) const;

// command interface
public:
	explicit PinholeCamera(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
	static std::unique_ptr<PinholeCamera> ciLoad(const InputPacket& packet);
};

}// end namespace ph