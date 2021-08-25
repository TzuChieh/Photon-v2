#pragma once

#include "DataIO/SDL/Introspect/SDLIOContext.h"
#include "DataIO/FileSystem/Path.h"
#include "Common/assertion.h"

#include <utility>
#include <string>

namespace ph { class SceneDescription; }

namespace ph
{

/*! @brief Data that SDL input process can rely on.

All data in the input context may be accessed concurrently.

@note Modifications to this class must be ready for concurrent use cases.
*/
class SdlInputContext final : public SDLIOContext
{
public:
	SdlInputContext();

	SdlInputContext(
		const SceneDescription* scene,
		Path                    workingDirectory,
		const SdlClass*         srcClass);

	const SceneDescription* getRawScene() const;

private:
	const SceneDescription* m_rawScene;
};

// In-header Implementation:

inline SdlInputContext::SdlInputContext() :
	SDLIOContext(),
	m_rawScene(nullptr)
{}

inline SdlInputContext::SdlInputContext(
	const SceneDescription* const scene,
	Path                          workingDirectory,
	const SdlClass* const         srcClass) :

	SDLIOContext(std::move(workingDirectory), srcClass),

	m_rawScene(scene)
{
	PH_ASSERT(m_rawScene);
}

inline const SceneDescription* SdlInputContext::getRawScene() const
{
	return m_rawScene;
}

}// end namespace ph
