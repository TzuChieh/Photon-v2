//#include "Core/Receiver/EnvironmentCamera.h"
//#include "Common/assertion.h"
//#include "Math/constant.h"
//#include "Math/Geometry/TSphere.h"
//#include "Math/Random.h"
//#include "Core/Quantity/Time.h"
//#include "Core/Ray.h"
//#include "Math/TQuaternion.h"
//
//namespace ph
//{
//
//// Default resolution set to 2:1 (range of phi is 2x of theta)
//EnvironmentCamera::EnvironmentCamera() : 
//	EnvironmentCamera(
//		{0, 0, 0},
//		math::QuaternionR::makeNoRotation(),
//		{512, 256})
//{}
//
//EnvironmentCamera::EnvironmentCamera(
//	const math::Vector3R&    position,
//	const math::QuaternionR& rotation,
//	const math::Vector2S&    resolution) : 
//
//	Receiver(
//		position,
//		rotation,
//		resolution),
//
//	m_receiverToWorld(math::StaticRigidTransform::makeForward(m_receiverToWorldDecomposed))
//{}
//
//Spectrum EnvironmentCamera::receiveRay(const math::Vector2D& rasterCoord, Ray* const out_ray) const
//{
//	PH_ASSERT(out_ray);
//
//	const auto rasterRes = getRasterResolution();
//	PH_ASSERT_GT(rasterRes.x, 0);
//	PH_ASSERT_GT(rasterRes.y, 0);
//
//	// Warp raster xy to surface of a unit sphere
//	const auto phi         = math::constant::two_pi<float64> * rasterCoord.x / static_cast<float64>(rasterRes.x);
//	const auto theta       = math::constant::pi<float64> * (1.0 - rasterCoord.y / static_cast<float64>(rasterRes.y));
//	const auto localOutDir = math::TSphere<float64>::makeUnit().phiThetaToSurface({phi, theta});
//
//	math::Vector3R worldOutDir;
//	m_receiverToWorld.transformV(math::Vector3R(localOutDir), &worldOutDir);
//
//	out_ray->setDirection(worldOutDir.normalize().negate());
//	out_ray->setOrigin(getPosition());
//	out_ray->setMinT(0.0001_r);// HACK: hard-coded number
//	out_ray->setMaxT(std::numeric_limits<real>::max());
//
//	// HACK
//	Time time;
//	time.relativeT = math::Random::genUniformReal_i0_e1();
//	out_ray->setTime(time);
//
//	PH_ASSERT_MSG(out_ray->getOrigin().isFinite() && out_ray->getDirection().isFinite(), "\n"
//		"origin    = " + out_ray->getOrigin().toString() + "\n"
//		"direction = " + out_ray->getDirection().toString() + "\n");
//
//	return Spectrum(1);
//}
//
//void EnvironmentCamera::evalEmittedImportanceAndPdfW(const math::Vector3R& targetPos, math::Vector2R* const out_filmCoord, math::Vector3R* const out_importance, real* out_filmArea, real* const out_pdfW) const
//{
//	PH_ASSERT_UNREACHABLE_SECTION();
//}
//
//}// end namespace ph
