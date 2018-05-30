#pragma once

#include "Common/primitive_type.h"
#include "FileIO/SDL/TCommandInterface.h"
#include "Core/Bound/TAABB2D.h"
#include "Math/TVector2.h"
#include "Frame/frame_fwd.h"

namespace ph
{

class InputPacket;

class Film : public TCommandInterface<Film>
{
public:
	Film(int64 actualWidthPx, int64 actualHeightPx);

	Film(
		int64 actualWidthPx, int64 actualHeightPx,
		const TAABB2D<int64>& effectiveWindowPx);

	virtual ~Film() = 0;

	virtual void clear() = 0;

	void develop(HdrRgbFrame& out_frame) const;
	void develop(HdrRgbFrame& out_frame, const TAABB2D<int64>& regionPx) const;

	inline const TVector2<int64>& getActualResPx() const
	{
		return m_actualResPx;
	}

	inline const TVector2<int64>& getEffectiveResPx() const
	{
		return m_effectiveResPx;
	}

	inline const TAABB2D<int64>& getEffectiveWindowPx() const
	{
		return m_effectiveWindowPx;
	}

protected:
	TVector2<int64> m_actualResPx;
	TVector2<int64> m_effectiveResPx;
	TAABB2D<int64>  m_effectiveWindowPx;

private:
	virtual void developRegion(HdrRgbFrame& out_frame, const TAABB2D<int64>& regionPx) const = 0;

// command interface
public:
	explicit Film(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
};

}// end namespace ph