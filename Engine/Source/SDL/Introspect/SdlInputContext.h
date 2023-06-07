#pragma once

#include "SDL/Introspect/SdlIOContext.h"
#include "DataIO/FileSystem/Path.h"
#include "Common/assertion.h"

#include <utility>
#include <string>

namespace ph { class SceneDescription; }

namespace ph
{

/*! @brief Data that SDL input process can rely on.
@note Modifications to this class must be aware for potential concurrent use cases.
*/
class SdlInputContext final : public SdlIOContext
{
public:
	SdlInputContext();

	SdlInputContext(
		Path workingDirectory,
		const SdlClass* srcClass);

	SdlInputContext(
		const SceneDescription* scene,
		Path workingDirectory,
		const SdlClass* srcClass);

	const SceneDescription* getRawScene() const;

private:
	const SceneDescription* m_rawScene;
};

// In-header Implementation:

inline SdlInputContext::SdlInputContext()
	: SdlIOContext()
	, m_rawScene(nullptr)
{}

inline SdlInputContext::SdlInputContext(
	Path workingDirectory,
	const SdlClass* const srcClass)

	: SdlInputContext(nullptr, std::move(workingDirectory), srcClass)
{}

inline SdlInputContext::SdlInputContext(
	const SceneDescription* const scene,
	Path workingDirectory,
	const SdlClass* const srcClass)

	: SdlIOContext(std::move(workingDirectory), srcClass)

	, m_rawScene(scene)
{}

inline const SceneDescription* SdlInputContext::getRawScene() const
{
	return m_rawScene;
}

}// end namespace ph
