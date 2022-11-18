#pragma once

#include "RenderCore/ghi_enums.h"
#include "RenderCore/ghi_states.h"
#include "RenderCore/GHITexture2D.h"

#include <Utility/INoCopyAndMove.h>
#include <Common/logging.h>
#include <Common/assertion.h>
#include <Common/primitive_type.h>
#include <Math/TVector4.h>

#include <source_location>
#include <type_traits>
#include <string>
#include <memory>

namespace ph::editor
{

PH_DECLARE_LOG_GROUP(GHI);

/*! @brief Graphics API abstraction.
@exception PlatformException When error occurred and the platform must terminate its operations.
*/
class GHI : private INoCopyAndMove
{
public:
	static std::string toString(EGraphicsAPI apiType);

public:
	explicit GHI(EGraphicsAPI apiType);
	virtual ~GHI();

	/*! @brief Load and initiate the GHI.
	load() and unload() must be called on the same thread. All remaining methods must be 
	called between calls to load() and unload() (except for ctor and dtor).
	*/
	virtual void load() = 0;

	/*! @brief Unload and clean up the GHI.
	Must be called on the same thread as load(). See load() for more information on the 
	requirements of this method.
	*/
	virtual void unload() = 0;

	/* @brief Set how NDC corresponds to pixels on the display.
	@param xPx X coordinate of the lower-left corner of the viewport, in pixels.
	@param yPx Y coordinate of the lower-left corner of the viewport, in pixels.
	@param widthPx Width of the viewport, in pixels.
	@param heightPx Height of the viewport, in pixels.
	*/
	virtual void setViewport(uint32 xPx, uint32 yPx, uint32 widthPx, uint32 heightPx) = 0;

	virtual void clearBuffer(EClearTarget target) = 0;

	virtual void setClearColor(const math::Vector4F& color) = 0;

	virtual void swapBuffers() = 0;

	virtual std::shared_ptr<GHITexture2D> createTexture2D(
		const math::TVector2<uint32>& sizePx, 
		EGHITextureFormat format,
		const GHISampleState& state) = 0;

	template<EGraphicsAPI API_TYPE, typename CommandCallingFunctor>
	void rawCommand(
		CommandCallingFunctor functor,
		std::source_location  srcLocation = std::source_location::current());

private:
	/*!
	Allows the implementer to save/cache internal states before raw commands are inserted manually, 
	and restore the states afterwards.
	*/
	///@{
	virtual void beginRawCommand() = 0;
	virtual void endRawCommand() = 0;
	///@}

	EGraphicsAPI m_apiType;
};

template<EGraphicsAPI API_TYPE, typename CommandCallingFunctor>
inline void GHI::rawCommand(
	CommandCallingFunctor      functor,
	const std::source_location srcLocation)
{
	static_assert(std::is_invocable_v<CommandCallingFunctor>,
		"must be callable without any argument");

	if(API_TYPE != m_apiType)
	{
		PH_LOG_ERROR(GHI,
			"cannot call raw command of type {} under current GHI type {}, "
			"skipping command at {}({}:{})",
			toString(API_TYPE), toString(m_apiType), srcLocation.file_name(), srcLocation.line(), srcLocation.column());
		
		return;
	}

	beginRawCommand();
	functor();
	endRawCommand();
}

}// end namespace ph::editor
