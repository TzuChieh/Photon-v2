#pragma once

#include "SDL/Introspect/SdlIOContext.h"
#include "DataIO/FileSystem/Path.h"
#include "SDL/SdlDependencyResolver.h"
#include "Common/assertion.h"

#include <utility>
#include <string>

namespace ph
{

/*! @brief Data that SDL output process can rely on.
@note Modifications to this class must be aware for potential concurrent use cases.
*/
class SdlOutputContext final : public SdlIOContext
{
public:
	SdlOutputContext();

	explicit SdlOutputContext(Path workingDirectory);

	SdlOutputContext(
		const SdlDependencyResolver* dependencyResolver,
		Path workingDirectory,
		const SdlClass* srcClass);

	const SdlDependencyResolver& getDependencyResolver() const;

private:
	const SdlDependencyResolver* m_dependencyResolver;
};

// In-header Implementation:

inline SdlOutputContext::SdlOutputContext() :
	SdlIOContext(),
	m_dependencyResolver(nullptr)
{}

inline SdlOutputContext::SdlOutputContext(Path workingDirectory)
	: SdlIOContext(std::move(workingDirectory))
	, m_dependencyResolver(nullptr)
{}

inline SdlOutputContext::SdlOutputContext(
	const SdlDependencyResolver* const dependencyResolver,
	Path workingDirectory,
	const SdlClass* const srcClass) :

	SdlIOContext(
		std::move(workingDirectory), 
		srcClass),

	m_dependencyResolver(dependencyResolver)
{}

inline const SdlDependencyResolver& SdlOutputContext::getDependencyResolver() const
{
	PH_ASSERT(m_dependencyResolver);
	return *m_dependencyResolver;
}

}// end namespace ph
