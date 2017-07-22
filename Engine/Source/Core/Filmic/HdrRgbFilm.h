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
	HdrRgbFilm(uint64 widthPx, uint64 heightPx,
	           const std::shared_ptr<SampleFilter>& filter);
	virtual ~HdrRgbFilm() override;

	//void accumulateRadiance(const uint32 x, const uint32 y, const Vector3R& radiance);
	//void accumulateRadiance(const Film& other);
	virtual void addSample(float64 xPx, float64 yPx, const Vector3R& radiance) override;
	virtual void develop(Frame* out_frame) const override;
	virtual void clear() override;
	virtual std::unique_ptr<Film> genChild(uint32 widthPx, uint32 heightPx) override;

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