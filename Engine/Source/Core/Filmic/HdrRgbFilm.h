#pragma once

#include "Common/primitive_type.h"
#include "Math/math_fwd.h"
#include "Core/Camera/RadianceSensor.h"
#include "FileIO/SDL/ISdlResource.h"
#include "FileIO/SDL/TCommandInterface.h"
#include "Core/Filmic/Film.h"

#include <vector>

namespace ph
{

class Frame;
class InputPacket;

class HdrRgbFilm final : public Film, public TCommandInterface<HdrRgbFilm>
{
public:
	HdrRgbFilm(int64 actualWidthPx, int64 actualHeightPx,
	           const std::shared_ptr<SampleFilter>& filter);
	HdrRgbFilm(int64 actualWidthPx, int64 actualHeightPx,
	           const TAABB2D<int64>& effectiveWindowPx,
	           const std::shared_ptr<SampleFilter>& filter);
	virtual ~HdrRgbFilm() override;

	//void accumulateRadiance(const uint32 x, const uint32 y, const Vector3R& radiance);
	//void accumulateRadiance(const Film& other);
	virtual void addSample(float64 xPx, float64 yPx, const Vector3R& radiance) override;
	virtual void clear() override;
	virtual std::unique_ptr<Film> genChild(const TAABB2D<int64>& effectiveWindowPx) override;

	// HACK
	/*inline void accumulateRadianceWithoutIncrementSenseCount(const uint32 x, const uint32 y, const Vector3R& radiance)
	{
		const std::size_t baseIndex = y * static_cast<std::size_t>(m_widthPx) + x;

		m_pixelRadianceSensors[baseIndex].m_accuR += static_cast<float64>(radiance.x);
		m_pixelRadianceSensors[baseIndex].m_accuG += static_cast<float64>(radiance.y);
		m_pixelRadianceSensors[baseIndex].m_accuB += static_cast<float64>(radiance.z);
	}*/

	// HACK
	/*inline void incrementAllSenseCounts()
	{
		for(auto& sensor : m_pixelRadianceSensors)
		{
			sensor.accuWeight += 1.0;
		}
	}*/

private:
	virtual void developRegion(Frame& out_frame, const TAABB2D<int64>& regionPx) const override;

	std::vector<RadianceSensor> m_pixelRadianceSensors;

// command interface
public:
	static SdlTypeInfo ciTypeInfo();
	static std::unique_ptr<HdrRgbFilm> ciLoad(const InputPacket& packet);
	static ExitStatus ciExecute(const std::shared_ptr<HdrRgbFilm>& targetResource, 
	                            const std::string& functionName, 
	                            const InputPacket& packet);
};

}// end namespace ph