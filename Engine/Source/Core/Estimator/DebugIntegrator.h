//#pragma once
//
//#include "Core/Integrator/Integrator.h"
//
//namespace ph
//{
//
//class DebugIntegrator final : public Integrator, public TCommandInterface<DebugIntegrator>
//{
//public:
//	virtual ~DebugIntegrator() override;
//
//	virtual void update(const Scene& scene) override;
//	virtual void radianceAlongRay(const Ray& ray, const RenderWork& data, std::vector<SenseEvent>& out_senseEvents) const override;
//
//// command interface
//public:
//	DebugIntegrator(const InputPacket& packet);
//	static SdlTypeInfo ciTypeInfo();
//	static void ciRegister(CommandRegister& cmdRegister);
//	static std::unique_ptr<DebugIntegrator> ciLoad(const InputPacket& packet);
//};
//
//}// end namespace ph
