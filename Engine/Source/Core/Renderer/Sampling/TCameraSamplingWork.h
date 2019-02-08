#pragma once

#include "Core/Renderer/RenderWork.h"
#include "Utility/INoncopyable.h"
#include "Core/Renderer/Sampling/SamplingStatistics.h"
#include "Core/SampleGenerator/SampleGenerator.h"
#include "Core/Renderer/Sampling/TSensedRayProcessor.h"
#include "Math/TVector2.h"
#include "Core/Bound/TAABB2D.h"

#include <atomic>
#include <functional>
#include <type_traits>

namespace ph
{

class Camera;

template<typename Processor>
class TCameraSamplingWork : public RenderWork, public INoncopyable
{
	static_assert(std::is_base_of_v<TSensedRayProcessor<Processor>, Processor>);

public:
	TCameraSamplingWork();

	TCameraSamplingWork(
		const Camera* camera,
		Processor*    processor);
	
	TCameraSamplingWork(TCameraSamplingWork&& other);

	SamplingStatistics asyncGetStatistics();

	void setSampleGenerator(std::unique_ptr<SampleGenerator> sampleGenerator);

	void setFilmDimensions(
		const Vector2S&         actualResPx, 
		const TAABB2D<int64>&   effectiveWindowPx,
		const TAABB2D<float64>& sampleWindowPx);

	void onWorkStart(std::function<void()> func);
	void onWorkReport(std::function<void()> func);
	void onWorkFinish(std::function<void()> func);

	TCameraSamplingWork& operator = (TCameraSamplingWork&& other);

private:
	void doWork() override;

	const Camera*                    m_camera;
	Processor*                       m_processor;
	std::unique_ptr<SampleGenerator> m_sampleGenerator;
	Vector2D                         m_filmActualResPx;
	TAABB2D<int64>                   m_filmEffectiveWindowPx;
	TAABB2D<float64>                 m_filmSampleWindowPx;

	std::atomic_uint32_t m_numSamplesTaken;
	std::function<void()> m_onWorkStart;
	std::function<void()> m_onWorkReport;
	std::function<void()> m_onWorkFinish;
};

}// end namespace ph

#include "Core/Renderer/Sampling/TCameraSamplingWork.ipp"