#include "SDL/TSdl.h"
#include "SDL/sdl_helpers.h"
#include "SDL/ISdlResource.h"
#include "SDL/SdlSceneFileReader.h"
#include "SDL/SdlSceneFileWriter.h"
#include "SDL/SceneDescription.h"
#include "SDL/SdlInlinePacketInterface.h"

#include <Common/assertion.h>
#include <Common/logging.h>

namespace ph
{

namespace detail
{

// Loading a SDL resource from file. Implemented in .cpp so the templated main interface will
// not drag I/O related dependencies into its header.
std::shared_ptr<ISdlResource> load_single_resource(const SdlClass* resourceClass, const Path& file)
{
	PH_ASSERT(resourceClass);

	SceneDescription description;
	{
		auto targetClasses = {resourceClass};
		SdlSceneFileReader reader(
			targetClasses,
			file,
			file.getParent());

		reader.read(&description);
	}
	
	auto allResources = description.getResources().getAllOfType<ISdlResource>();
	if(allResources.size() != 1)
	{
		PH_DEFAULT_LOG_WARNING(
			"Expecting 1 resource, {} were found",
			allResources.size());
	}

	if(allResources.empty())
	{
		return nullptr;
	}
	else
	{
		auto loadedResource = allResources.front();
		if(loadedResource->getDynamicSdlClass() != resourceClass)
		{
			PH_DEFAULT_LOG_WARNING(
				"Resource type mismatch, expecting: {}, found: {}",
				sdl::gen_pretty_name(loadedResource->getDynamicSdlClass()),
				sdl::gen_pretty_name(resourceClass));
			return nullptr;
		}

		return loadedResource;
	}
}

// Saving a SDL resource to file. Implemented in .cpp so the templated main interface will
// not drag I/O related dependencies into its header.
void save_single_resource(const std::shared_ptr<ISdlResource>& resource, const Path& file)
{
	PH_ASSERT(resource);

	SceneDescription description;

	// Resource name does not matter
	description.getResources().add(resource, "res");

	auto targetClasses = {resource->getDynamicSdlClass()};
	SdlSceneFileWriter writer(
		targetClasses,
		file,
		file.getParent());

	// For single-resource files, lay each clause on a separate line for ease of reading/editing
	writer.setPacketInterface(std::make_unique<SdlInlinePacketInterface>(0, ' '));

	writer.write(description);
}

}// end namespace detail

}// end namespace ph
