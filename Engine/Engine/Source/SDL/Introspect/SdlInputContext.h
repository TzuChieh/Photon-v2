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
		const ISdlReferenceGroup* srcReferences,
		Path workingDirectory,
		const SdlClass* srcClass);

	/*!
	@return References that are relevant to the current input.
	*/
	const ISdlReferenceGroup* getSrcReferences() const;

private:
	const ISdlReferenceGroup* m_srcReferences;
};

// In-header Implementation:

inline SdlInputContext::SdlInputContext()
	: SdlIOContext()
	, m_srcReferences(nullptr)
{}

inline SdlInputContext::SdlInputContext(Path workingDirectory)
	: SdlIOContext(std::move(workingDirectory))
	, m_srcReferences(nullptr)
{}

inline SdlInputContext::SdlInputContext(
	Path workingDirectory,
	const SdlClass* const srcClass)

	: SdlInputContext(nullptr, std::move(workingDirectory), srcClass)
{}

inline SdlInputContext::SdlInputContext(
	const ISdlReferenceGroup* const srcReferences,
	Path workingDirectory,
	const SdlClass* const srcClass)

	: SdlIOContext(std::move(workingDirectory), srcClass)

	, m_srcReferences(srcReferences)
{}

inline const ISdlReferenceGroup* SdlInputContext::getSrcReferences() const
{
	return m_srcReferences;
}

}// end namespace ph
