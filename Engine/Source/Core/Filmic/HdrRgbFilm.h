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

	virtual void addSample(float64 xPx, float64 yPx, const SpectralStrength& radiance) override;
	virtual void clear() override;
	virtual std::unique_ptr<Film> genChild(const TAABB2D<int64>& effectiveWindowPx) override;

private:
	virtual void developRegion(Frame& out_frame, const TAABB2D<int64>& regionPx) const override;

	std::vector<RadianceSensor> m_pixelRadianceSensors;

	void mergeWith(const HdrRgbFilm& other);

// command interface
public:
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
	static std::unique_ptr<HdrRgbFilm> ciLoad(const InputPacket& packet);
};

}// end namespace ph