#pragma once

#include "Common/primitive_type.h"
#include "Math/math_fwd.h"
#include "Core/Camera/RadianceSensor.h"
#include "FileIO/SDL/ISdlResource.h"
#include "FileIO/SDL/TCommandInterface.h"

#include <vector>
#include <functional>
#include <memory>

namespace ph
{

class Frame;
class InputPacket;
class SampleFilter;

class Film : public TCommandInterface<Film>, public ISdlResource
{
public:
	Film(uint32 widthPx, uint32 heightPx, 
	     const std::shared_ptr<SampleFilter>& filter);
	virtual ~Film() = 0;

	virtual void addSample(float64 xPx, float64 yPx, const Vector3R& radiance) = 0;
	virtual void develop(Frame* out_frame) const = 0;
	virtual void clear() = 0;
	virtual std::unique_ptr<Film> genChild(uint32 widthPx, uint32 heightPx) = 0;

	inline void mergeToParent() const
	{
		m_merger();
	}

	inline uint32 getWidthPx() const
	{
		return m_widthPx;
	}

	inline uint32 getHeightPx() const
	{
		return m_heightPx;
	}

protected:
	uint32 m_widthPx;
	uint32 m_heightPx;
	std::shared_ptr<SampleFilter> m_filter;
	std::function<void()> m_merger;

// command interface
public:
	Film(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static ExitStatus ciExecute(const std::shared_ptr<Film>& targetResource, 
	                            const std::string& functionName, 
	                            const InputPacket& packet);
};

}// end namespace ph