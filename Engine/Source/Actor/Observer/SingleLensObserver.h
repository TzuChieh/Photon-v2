#pragma once

#include "Actor/Observer/OrientedRasterObserver.h"
#include "DataIO/SDL/sdl_interface.h"
#include "Common/primitive_type.h"
#include "Math/Transform/TDecomposedTransform.h"
#include "Math/TVector2.h"

namespace ph { class PinholeCamera; }
namespace ph { class ThinLensCamera; }

namespace ph
{

class SingleLensObserver : public OrientedRasterObserver
{
public:
	inline SingleLensObserver() = default;

	std::unique_ptr<Receiver> genReceiver(const CoreCookingContext& ctx) override;

protected:
	math::TDecomposedTransform<float64> makeRasterToObserver() const;
	math::Vector2D getSensorSize() const;
	float64 getSensorOffset() const;
	void genPinholeCamera(const CoreCookingContext& ctx, PinholeCamera* out_pinholeCamera);
	void genThinLensCamera(const CoreCookingContext& ctx, ThinLensCamera* out_thinLensCamera);

private:
	real m_lensRadiusMM;
	real m_focalDistanceMM;
	real m_sensorWidthMM;
	real m_sensorOffsetMM;

public:
	PH_DEFINE_SDL_CLASS(TOwnerSdlClass<SingleLensObserver>)
	{
		ClassType clazz("single-lens");
		clazz.description(
			"As its name suggests, the lens system in this observer is assumed to have "
			"just a single lens. The biggest advantage of it is that depth of field "
			"effects are possible under this model. In case of the lens radius is zero, "
			"the lens system will be reduced to a pinhole. Images captured by this "
			"observer is similar to how a normal human perceives the world but with "
			"several simplifications.");
		clazz.baseOn<OrientedRasterObserver>();

		TSdlReal<OwnerType> lensRadiusMM("lens-radius-mm", &OwnerType::m_lensRadiusMM);
		lensRadiusMM.description("Radius of the lens in millimeters.");
		lensRadiusMM.defaultTo(0);
		lensRadiusMM.optional();
		clazz.addField(lensRadiusMM);

		TSdlReal<OwnerType> focalDistanceMM("focal-distance-mm", &OwnerType::m_focalDistanceMM);
		focalDistanceMM.description("The distance in millimeters that the observer is focusing on.");
		focalDistanceMM.defaultTo(150);
		focalDistanceMM.optional();
		clazz.addField(focalDistanceMM);

		TSdlReal<OwnerType> sensorWidthMM("sensor-width-mm", &OwnerType::m_sensorWidthMM);
		sensorWidthMM.description("Width of the sensor used by this observer in millimeters.");
		sensorWidthMM.defaultTo(36);
		sensorWidthMM.optional();
		clazz.addField(sensorWidthMM);

		TSdlReal<OwnerType> sensorOffsetMM("sensor-offset-mm", &OwnerType::m_sensorOffsetMM);
		sensorOffsetMM.description("Distance between sensor and light entry.");
		sensorOffsetMM.defaultTo(36);
		sensorOffsetMM.optional();
		clazz.addField(sensorOffsetMM);

		return clazz;
	}
};

}// end namespace ph
