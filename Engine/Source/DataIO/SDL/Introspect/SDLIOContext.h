#pragma once

#include "DataIO/FileSystem/Path.h"

#include <utility>
#include <string>

namespace ph { class SdlClass; }

namespace ph
{

/*! @brief Data that SDL input/output process can rely on.

All data in the context may be accessed concurrently.

@note Modifications to this class must be ready for concurrent use cases.
*/
class SDLIOContext
{
public:
	std::string genPrettySrcClassName() const;
	const Path& getWorkingDirectory() const;

	/*! @brief The SDL class that was originally involved in an input process.
	@return The source SDL class. May be empty if such information cannot be obtained.
	*/
	const SdlClass* getSrcClass() const;

// Protected, to prevent certain polymorphic usage
protected:
	SDLIOContext();

	/*! 
	@param workingDirectory Working directory of the current SDL IO process. Note that
	SDL working directory can be different to the program's working directory. It is specific
	to the related SDL command.
	*/
	SDLIOContext(
		Path            workingDirectory,
		const SdlClass* srcClass);

	inline ~SDLIOContext() = default;

	inline SDLIOContext(const SDLIOContext& other) = default;
	inline SDLIOContext& operator = (const SDLIOContext& rhs) = default;

private:
	Path            m_workingDirectory;
	const SdlClass* m_srcClass;
};

// In-header Implementation:

inline SDLIOContext::SDLIOContext() :
	m_workingDirectory(),
	m_srcClass        (nullptr)
{}

inline SDLIOContext::SDLIOContext(
	Path                  workingDirectory,
	const SdlClass* const srcClass) :

	m_workingDirectory(std::move(workingDirectory)),
	m_srcClass        (srcClass)
{}

inline const Path& SDLIOContext::getWorkingDirectory() const
{
	return m_workingDirectory;
}

inline const SdlClass* SDLIOContext::getSrcClass() const
{
	return m_srcClass;
}

}// end namespace ph
