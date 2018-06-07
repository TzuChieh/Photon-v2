#pragma once

#include "Core/Integrator/Integrator.h"
#include "Core/Bound/TAABB2D.h"
#include "Core/Filmic/TSamplingFilm.h"
#include "Core/Quantity/SpectralStrength.h"
#include "Core/SurfaceHit.h"
#include "Utility/INoncopyable.h"

#include <memory>

namespace ph
{

class Scene;
class Camera;

class AbstractPathIntegrator : public Integrator
{
public:
	AbstractPathIntegrator();
	~AbstractPathIntegrator() override = 0;

	std::unique_ptr<Integrator> makeCopy() const override = 0;

	AttributeTags supportedAttributes() const override;
	void setDomainPx(const TAABB2D<int64>& domain) override;
	void setIntegrand(const RenderWork& integrand) override;
	void integrate(const AttributeTags& requestedAttributes) override;
	void asyncGetDomainAttribute(EAttribute target, HdrRgbFrame& out_frame) override;

protected:
	AbstractPathIntegrator(const AbstractPathIntegrator& other);

	AbstractPathIntegrator& operator = (const AbstractPathIntegrator& rhs);

private:
	virtual void tracePath(
		const Ray&        ray, 
		SpectralStrength* out_lightEnergy, 
		SurfaceHit*       out_firstHit) const = 0;

	const Scene*   m_scene;
	const Camera*  m_camera;
	TAABB2D<int64> m_domainPx;

	std::unique_ptr<TSamplingFilm<SpectralStrength>> m_lightEnergy;

	void initFilms();

	friend void swap(AbstractPathIntegrator& first, AbstractPathIntegrator& second);
};

}// end namespace ph