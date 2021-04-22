#pragma once

#include "DataIO/FileSystem/Path.h"
#include "Common/assertion.h"

#include <utility>
#include <string>

namespace ph
{

class SdlClass;
class NamedResourceStorage;

class SdlInputContext final
{
public:
	SdlInputContext();

	SdlInputContext(
		const NamedResourceStorage* resources,
		Path                        workingDirectory,
		const SdlClass*             srcClass);

	std::string genPrettySrcClassName() const;

public:
	Path                        workingDirectory;
	const SdlClass*             srcClass;
	const NamedResourceStorage* resources;
};

// In-header Implementation:

inline SdlInputContext::SdlInputContext() :
	resources       (nullptr),
	workingDirectory(),
	srcClass        (nullptr)
{}

inline SdlInputContext::SdlInputContext(
	const NamedResourceStorage* resources,
	Path                        workingDirectory,
	const SdlClass* const       srcClass) :

	resources        (nullptr),
	workingDirectory (std::move(workingDirectory)),
	srcClass         (srcClass)
{
	PH_ASSERT(resources);
}

}// end namespace ph
