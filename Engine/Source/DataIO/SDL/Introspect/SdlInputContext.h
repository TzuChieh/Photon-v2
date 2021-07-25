#pragma once

#include "DataIO/FileSystem/Path.h"
#include "Common/assertion.h"

#include <utility>
#include <string>

namespace ph
{

class SdlClass;
class SceneDescription;

/*! @brief Data that SDL input process can rely on.

All data in the input context may be accessed concurrently.

@note Modifications to this class must be ready for concurrent access.
*/
class SdlInputContext final
{
public:
	SdlInputContext();

	SdlInputContext(
		const SceneDescription* scene,
		Path                    workingDirectory,
		const SdlClass*         srcClass);

	std::string genPrettySrcClassName() const;
	const Path& getWorkingDirectory() const;
	const SdlClass* getSrcClass() const;
	const SceneDescription* getRawScene() const;

private:
	Path                    m_workingDirectory;
	const SdlClass*         m_srcClass;
	const SceneDescription* m_rawScene;
};

// In-header Implementation:

inline SdlInputContext::SdlInputContext() :
	m_workingDirectory(),
	m_srcClass        (nullptr),
	m_rawScene        (nullptr)
{}

inline SdlInputContext::SdlInputContext(
	const SceneDescription* const scene,
	Path                          workingDirectory,
	const SdlClass* const         srcClass) :

	m_workingDirectory(std::move(workingDirectory)),
	m_srcClass        (srcClass),
	m_rawScene        (scene)
{
	PH_ASSERT(m_rawScene);
}

inline const Path& SdlInputContext::getWorkingDirectory() const
{
	return m_workingDirectory;
}

inline const SdlClass* SdlInputContext::getSrcClass() const
{
	return m_srcClass;
}

inline const SceneDescription* SdlInputContext::getRawScene() const
{
	return m_rawScene;
}

}// end namespace ph
