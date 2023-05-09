#pragma once

#include "DataIO/FileSystem/Path.h"

#include <utility>
#include <string>

namespace ph { class SdlClass; }

namespace ph
{

/*! @brief Data that SDL input/output process can rely on.
@note Modifications to this class must be aware for potential concurrent use cases.
*/
class SdlIOContext
{
public:
	std::string genPrettySrcClassName() const;
	const Path& getWorkingDirectory() const;

	/*! @brief The SDL class that was originally involved in an input process.
	@return The source SDL class. May be empty if such information cannot be obtained.
	*/
	const SdlClass* getSrcClass() const;

	void setSrcClass(const SdlClass* srcClass);

// Protected, to prevent certain polymorphic usage
protected:
	SdlIOContext();

	/*! 
	@param workingDirectory Working directory of the current SDL IO process. Note that
	SDL working directory can be different to the program's working directory. It is specific
	to the related SDL command.
	*/
	SdlIOContext(
		Path            workingDirectory,
		const SdlClass* srcClass);

	inline ~SdlIOContext() = default;

	inline SdlIOContext(const SdlIOContext& other) = default;
	inline SdlIOContext& operator = (const SdlIOContext& rhs) = default;

	inline SdlIOContext(SdlIOContext&& other) = default;
	inline SdlIOContext& operator = (SdlIOContext&& rhs) = default;

private:
	Path            m_workingDirectory;
	const SdlClass* m_srcClass;
};

// In-header Implementation:

inline SdlIOContext::SdlIOContext() :
	m_workingDirectory(),
	m_srcClass        (nullptr)
{}

inline SdlIOContext::SdlIOContext(
	Path                  workingDirectory,
	const SdlClass* const srcClass) :

	m_workingDirectory(std::move(workingDirectory)),
	m_srcClass        (srcClass)
{}

inline const Path& SdlIOContext::getWorkingDirectory() const
{
	return m_workingDirectory;
}

inline const SdlClass* SdlIOContext::getSrcClass() const
{
	return m_srcClass;
}

inline void SdlIOContext::setSrcClass(const SdlClass* const srcClass)
{
	m_srcClass = srcClass;
}

}// end namespace ph
