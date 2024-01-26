#pragma once

#include "SDL/Introspect/SdlIOContext.h"
#include "SDL/sdl_fwd.h"

#include <utility>
#include <string>
#include <string_view>
#include <type_traits>

namespace ph { class Path; }

namespace ph
{

/*! @brief Data that SDL output process can rely on.
*/
class SdlOutputContext final : public SdlIOContext
{
public:
	SdlOutputContext();

	explicit SdlOutputContext(
		const Path* workingDirectory,
		const SdlClass* srcClass = nullptr,
		const SdlStruct* srcStruct = nullptr);

	SdlOutputContext(
		const SdlDependencyResolver* dependencyResolver,
		SdlNamedOutputClauses* namedOutputClauses,
		const Path* workingDirectory,
		const SdlClass* srcClass = nullptr,
		const SdlStruct* srcStruct = nullptr);

	const SdlDependencyResolver* getDependencyResolver() const;
	SdlNamedOutputClauses* getNamedOutputClauses() const;

	/*!
	@return Resource name if such information exists. May be empty if `resource` is null or no name
	information is available.
	*/
	std::string_view getResourceName(const ISdlResource* resource) const;

private:
	const SdlDependencyResolver* m_dependencyResolver;
	SdlNamedOutputClauses* m_namedOutputClauses;
};

// Try to make the context trivially copyable, so mock contexts and copy-and-modified contexts can
// be cheaper to create.
static_assert(std::is_trivially_copyable_v<SdlOutputContext>);

// In-header Implementation:

inline SdlOutputContext::SdlOutputContext()

	: SdlIOContext()

	, m_dependencyResolver(nullptr)
	, m_namedOutputClauses(nullptr)
{}

inline SdlOutputContext::SdlOutputContext(
	const Path* const workingDirectory,
	const SdlClass* const srcClass,
	const SdlStruct* const srcStruct)

	: SdlOutputContext(
		nullptr,
		nullptr,
		workingDirectory,
		srcClass,
		srcStruct)
{}

inline SdlOutputContext::SdlOutputContext(
	const SdlDependencyResolver* const dependencyResolver,
	SdlNamedOutputClauses* const namedOutputClauses,
	const Path* const workingDirectory,
	const SdlClass* const srcClass,
	const SdlStruct* const srcStruct)

	: SdlIOContext(
		workingDirectory,
		srcClass,
		srcStruct)

	, m_dependencyResolver(dependencyResolver)
	, m_namedOutputClauses(namedOutputClauses)
{}

inline const SdlDependencyResolver* SdlOutputContext::getDependencyResolver() const
{
	return m_dependencyResolver;
}

inline SdlNamedOutputClauses* SdlOutputContext::getNamedOutputClauses() const
{
	return m_namedOutputClauses;
}

}// end namespace ph
