#pragma once

#include "Core/Integrator/Integrator.h"
#include "Core/Bound/TAABB2D.h"
#include "Core/Filmic/TSamplingFilm.h"
#include "Core/Quantity/SpectralStrength.h"
#include "Core/SurfaceHit.h"
#include "Utility/INoncopyable.h"
#include "Core/Filmic/SampleFilter.h"

#include <memory>
#include <mutex>

namespace ph
{

class Scene;
class Camera;
class SampleGenerator;

class AbstractPathIntegrator : public Integrator, public TCommandInterface<AbstractPathIntegrator>
{
public:
	AbstractPathIntegrator();
	~AbstractPathIntegrator() override;

	std::unique_ptr<Integrator> makeReproduction() const override = 0;

	AttributeTags supportedAttributes() const override;
	void setDomainPx(const TAABB2D<int64>& domain, uint32 widthPx, uint32 heightPx) override;
	void setIntegrand(const RenderWork& integrand) override;
	void integrate(const AttributeTags& requestedAttributes) override;
	void asyncGetAttribute(EAttribute target, HdrRgbFrame& out_frame) override;

	friend void swap(AbstractPathIntegrator& first, AbstractPathIntegrator& second);

protected:
	AbstractPathIntegrator(const AbstractPathIntegrator& other);

	AbstractPathIntegrator& operator = (const AbstractPathIntegrator& rhs) = delete;

	const Scene*   m_scene;

private:
	virtual void tracePath(
		const Ray&        ray, 
		SpectralStrength* out_lightEnergy, 
		SurfaceHit*       out_firstHit) const = 0;

	
	const Camera*  m_camera;
	SampleGenerator* m_sg;
	TAABB2D<int64> m_domainPx;
	uint32 m_widthPx, m_heightPx;
	SampleFilter   m_filter;
	std::mutex m_filmMutex;

	std::unique_ptr<TSamplingFilm<SpectralStrength>> m_lightEnergy;

	bool initFilms();

// command interface
public:
	explicit AbstractPathIntegrator(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
	
	/*template<typename IntegratorType>
	static void registerExecutors(CommandRegister& cmdRegister);*/
};

// In-header Implementations:

//template<typename IntegratorType>
//inline void AbstractPathIntegrator::registerExecutors(CommandRegister& cmdRegister)
//{
//
//}

}// end namespace ph