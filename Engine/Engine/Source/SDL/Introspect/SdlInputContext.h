#pragma once

#include "SDL/Introspect/SdlIOContext.h"
#include "SDL/sdl_fwd.h"
#include "DataIO/FileSystem/Path.h"

#include <Common/assertion.h>

#include <utility>
#include <string>

namespace ph
{

/*! @brief Data that SDL input process can rely on.
@note Modifications to this class must be aware for potential concurrent use cases.
*/
class SdlInputContext final : public SdlIOContext
{
public:
	SdlInputContext();

	explicit SdlInputContext(Path workingDirectory);

	SdlInputContext(
		Path workingDirectory,
		const SdlClass* srcClass);

	SdlInputContext(
		const ISdlReferenceGroup* srcResources,
		const ISdlDataPacketGroup* srcDataPackets,
		Path workingDirectory,
		const SdlClass* srcClass);

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

// In-header Implementation:

inline SdlInputContext::SdlInputContext()
	: SdlIOContext()
	, m_srcResources(nullptr)
	, m_srcDataPackets(nullptr)
{}

inline SdlInputContext::SdlInputContext(Path workingDirectory)
	: SdlIOContext(std::move(workingDirectory))
	, m_srcResources(nullptr)
	, m_srcDataPackets(nullptr)
{}

inline SdlInputContext::SdlInputContext(
	Path workingDirectory,
	const SdlClass* const srcClass)

	: SdlInputContext(nullptr, nullptr, std::move(workingDirectory), srcClass)
{}

inline SdlInputContext::SdlInputContext(
	const ISdlReferenceGroup* const srcResources,
	const ISdlDataPacketGroup* const srcDataPackets,
	Path workingDirectory,
	const SdlClass* const srcClass)

	: SdlIOContext(std::move(workingDirectory), srcClass)

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
