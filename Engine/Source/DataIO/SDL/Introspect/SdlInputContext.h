#pragma once

#include "DataIO/FileSystem/Path.h"

#include <utility>

namespace ph
{

class SdlClass;

class SdlInputContext final
{
public:
	SdlInputContext();

	SdlInputContext(
		Path            workingDirectory,
		const SdlClass* srcClass);

public:
	Path            workingDirectory;
	const SdlClass* srcClass;
};

// In-header Implementation:

inline SdlInputContext::SdlInputContext() :
	workingDirectory(),
	srcClass        (nullptr)
{}

inline SdlInputContext::SdlInputContext(
	Path                  workingDirectory,
	const SdlClass* const srcClass) :

	workingDirectory (std::move(workingDirectory)),
	srcClass         (srcClass)
{}

}// end namespace ph
