#pragma once

#include "Core/Renderer/RenderWork.h"
#include "Utility/INoncopyable.h"
#include "Core/Renderer/Sampling/SamplingStatistics.h"
#include "Core/SampleGenerator/SampleGenerator.h"
#include "Core/Renderer/Sampling/ISensedRayProcessor.h"
#include "Math/TVector2.h"
#include "Math/Geometry/TAABB2D.h"
#include "Core/Filmic/SamplingFilmDimensions.h"

#include <atomic>
#include <functional>

namespace ph
{

class Camera;

class CameraSamplingWork : public RenderWork, public INoncopyable
{
public:
	template<typename T>
	using TAABB2D = math::TAABB2D<T>;

	CameraSamplingWork();
	explicit CameraSamplingWork(const Camera* camera);
	CameraSamplingWork(CameraSamplingWork&& other);

	SamplingStatistics asyncGetStatistics();

	void setSampleGenerator(std::unique_ptr<SampleGenerator> sampleGenerator);
	void setSampleDimensions(
		const TVector2<int64>&  filmResPx, 
		const TAABB2D<float64>& filmWindowPx,
		const TVector2<int64>&  sampleResPx);
	void addProcessor(ISensedRayProcessor* processor);

	void onWorkStart(std::function<void()> func);
	void onWorkReport(std::function<void()> func);
	void onWorkFinish(std::function<void()> func);

	CameraSamplingWork& operator = (CameraSamplingWork&& other);

private:
	void doWork() override;

	const Camera*                     m_camera;
	std::vector<ISensedRayProcessor*> m_processors;
	std::unique_ptr<SampleGenerator>  m_sampleGenerator;
	Vector2D                          m_filmResPx;
	TAABB2D<float64>                  m_filmWindowPx;
	Vector2S                          m_sampleResPx;

	std::atomic_uint32_t  m_numSamplesTaken;
	std::function<void()> m_onWorkStart;
	std::function<void()> m_onWorkReport;
	std::function<void()> m_onWorkFinish;
};

}// end namespace ph