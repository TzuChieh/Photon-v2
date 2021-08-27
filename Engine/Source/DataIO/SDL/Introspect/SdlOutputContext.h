#pragma once

#include "DataIO/SDL/Introspect/SdlIOContext.h"
#include "DataIO/FileSystem/Path.h"
#include "Common/assertion.h"

#include <utility>
#include <string>

namespace ph
{

/*! @brief Data that SDL output process can rely on.

All data in the output context may be accessed concurrently.

@note Modifications to this class must be ready for concurrent use cases.
*/
class SdlOutputContext final : public SdlIOContext
{
public:
	SdlOutputContext();

	SdlOutputContext(
		Path            workingDirectory,
		const SdlClass* srcClass);

private:
};

// In-header Implementation:

inline SdlOutputContext::SdlOutputContext() :
	SdlIOContext()
{}

inline SdlOutputContext::SdlOutputContext(
	Path                  workingDirectory,
	const SdlClass* const srcClass) :

	SdlIOContext(std::move(workingDirectory), srcClass)
{}

}// end namespace ph
