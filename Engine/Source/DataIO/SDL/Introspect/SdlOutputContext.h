#pragma once

#include "DataIO/SDL/Introspect/SdlIOContext.h"
#include "DataIO/FileSystem/Path.h"
#include "DataIO/SDL/SdlReferenceResolver.h"
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
		const SdlReferenceResolver* refResolver,
		Path                        workingDirectory,
		const SdlClass*             srcClass);

	const SdlReferenceResolver& getReferenceResolver() const;

private:
	const SdlReferenceResolver* m_refResolver;
};

// In-header Implementation:

inline SdlOutputContext::SdlOutputContext() :
	SdlIOContext(),
	m_refResolver(nullptr)
{}

inline SdlOutputContext::SdlOutputContext(
	const SdlReferenceResolver* const refResolver,
	Path                              workingDirectory,
	const SdlClass* const             srcClass) :

	SdlIOContext(
		std::move(workingDirectory), 
		srcClass),

	m_refResolver(refResolver)
{}

inline const SdlReferenceResolver& SdlOutputContext::getReferenceResolver() const
{
	PH_ASSERT(m_refResolver);
	return *m_refResolver;
}

}// end namespace ph
