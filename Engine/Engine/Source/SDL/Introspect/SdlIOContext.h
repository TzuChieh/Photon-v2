#pragma once

#include "SDL/sdl_fwd.h"

#include <Common/assertion.h>

#include <string>

namespace ph { class Path; }

namespace ph
{

/*! @brief Data that SDL input/output process can rely on.
*/
class SdlIOContext
{
public:
	std::string genPrettySrcInfo() const;

	/*! @brief Get working directory of the current I/O session.
	Do not treat this as the classical definition of working directory (for a process). 
	A SDL working directory may be different for each I/O session, and different to the working
	directory of the current process.
	*/
	const Path& getWorkingDirectory() const;

	/*! @brief The SDL class that is involved in the I/O process.
	@return The source SDL class. May be empty if no class is involved or such information cannot
	be obtained.
	*/
	const SdlClass* getSrcClass() const;

	const SdlStruct* getSrcStruct() const;

	void setSrcClass(const SdlClass* srcClass);
	void setSrcStruct(const SdlStruct* srcStruct);

// Protected, to prevent certain polymorphic usage
protected:
	SdlIOContext();

	/*! 
	@param workingDirectory Working directory of the current SDL IO process. Note that
	SDL working directory can be different to the program's working directory. It is specific
	to the related SDL command.
	*/
	explicit SdlIOContext(
		const Path*      workingDirectory,
		const SdlClass*  srcClass = nullptr,
		const SdlStruct* srcStruct = nullptr);

	SdlIOContext(const SdlIOContext& other) = default;
	SdlIOContext(SdlIOContext&& other) = default;
	SdlIOContext& operator = (const SdlIOContext& rhs) = default;
	SdlIOContext& operator = (SdlIOContext&& rhs) = default;

private:
	const Path*      m_workingDirectory;
	const SdlClass*  m_srcClass;
	const SdlStruct* m_srcStruct;
};

// In-header Implementation:

inline SdlIOContext::SdlIOContext()
	: m_workingDirectory(nullptr)
	, m_srcClass        (nullptr)
	, m_srcStruct       (nullptr)
{}

inline SdlIOContext::SdlIOContext(
	const Path* const      workingDirectory,
	const SdlClass* const  srcClass,
	const SdlStruct* const srcStruct)

	: m_workingDirectory(workingDirectory)
	, m_srcClass        (srcClass)
	, m_srcStruct       (srcStruct)
{
	PH_ASSERT(workingDirectory);
}

inline const Path& SdlIOContext::getWorkingDirectory() const
{
	PH_ASSERT(m_workingDirectory);
	return *m_workingDirectory;
}

inline const SdlClass* SdlIOContext::getSrcClass() const
{
	return m_srcClass;
}

inline const SdlStruct* SdlIOContext::getSrcStruct() const
{
	return m_srcStruct;
}

inline void SdlIOContext::setSrcClass(const SdlClass* const srcClass)
{
	m_srcClass = srcClass;
}

inline void SdlIOContext::setSrcStruct(const SdlStruct* const srcStruct)
{
	m_srcStruct = srcStruct;
}

}// end namespace ph
