//#pragma once
//
//#include "Core/Integrator/Integrator.h"
//#include "Math/math_fwd.h"
//
//namespace ph
//{
//
//// FIXME: this integrator is not maintained
//
//class LightTracingIntegrator final : public Integrator, public TCommandInterface<LightTracingIntegrator>
//{
//public:
//	virtual ~LightTracingIntegrator() override;
//
//	virtual void update(const Scene& scene) override;
//	virtual void radianceAlongRay(const Ray& ray, const RenderWork& data, std::vector<SenseEvent>& out_senseEvents) const override;
//
//private:
//	static void rationalClamp(Vector3R& value);
//
//// command interface
//public:
//	LightTracingIntegrator(const InputPacket& packet);
//	static SdlTypeInfo ciTypeInfo();
//	static void ciRegister(CommandRegister& cmdRegister);
//	static std::unique_ptr<LightTracingIntegrator> ciLoad(const InputPacket& packet);
//};
//
//}// end namespace ph
