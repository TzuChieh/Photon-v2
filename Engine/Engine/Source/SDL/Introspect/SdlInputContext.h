#pragma once

#include "SDL/Introspect/SdlIOContext.h"
#include "SDL/sdl_fwd.h"
#include "DataIO/FileSystem/Path.h"

#include <Common/assertion.h>

#include <utility>
#include <string>
#include <type_traits>

namespace ph
{

/*! @brief Data that SDL input process can rely on.
*/
class SdlInputContext final : public SdlIOContext
{
public:
	SdlInputContext();

	explicit SdlInputContext(
		const Path* workingDirectory,
		const SdlClass* srcClass = nullptr,
		const SdlStruct* srcStruct = nullptr);

	SdlInputContext(
		const ISdlReferenceGroup* srcResources,
		const ISdlDataPacketGroup* srcDataPackets,
		const Path* workingDirectory,
		const SdlClass* srcClass = nullptr,
		const SdlStruct* srcStruct = nullptr);

	/*!
	@return References that are relevant to the current input.
	*/
	const ISdlReferenceGroup* getSrcResources() const;

	/*!
	@return Named data packets that are relevant to the current input.
	*/
	const ISdlDataPacketGroup* getSrcDataPackets() const;

private:
	const ISdlReferenceGroup* m_srcResources;
	const ISdlDataPacketGroup* m_srcDataPackets;
};

// Try to make the context trivially copyable, so mock contexts and copy-and-modified contexts can
// be cheaper to create.
static_assert(std::is_trivially_copyable_v<SdlInputContext>);

// In-header Implementation:

inline SdlInputContext::SdlInputContext()

	: SdlIOContext()

	, m_srcResources(nullptr)
	, m_srcDataPackets(nullptr)
{}

inline SdlInputContext::SdlInputContext(
	const Path* const workingDirectory,
	const SdlClass* const srcClass,
	const SdlStruct* const srcStruct)

	: SdlInputContext(
		nullptr,
		nullptr,
		workingDirectory, 
		srcClass, 
		srcStruct)
{}

inline SdlInputContext::SdlInputContext(
	const ISdlReferenceGroup* const srcResources,
	const ISdlDataPacketGroup* const srcDataPackets,
	const Path* const workingDirectory,
	const SdlClass* const srcClass,
	const SdlStruct* const srcStruct)

	: SdlIOContext(
		workingDirectory, 
		srcClass, 
		srcStruct)

	, m_srcResources(srcResources)
	, m_srcDataPackets(srcDataPackets)
{}

inline const ISdlReferenceGroup* SdlInputContext::getSrcResources() const
{
	return m_srcResources;
}

inline const ISdlDataPacketGroup* SdlInputContext::getSrcDataPackets() const
{
	return m_srcDataPackets;
}

}// end namespace ph
