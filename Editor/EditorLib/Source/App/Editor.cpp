#include "App/Editor.h"
#include "ph_editor.h"
#include "EditorCore/Thread/Threads.h"
#include "Designer/DesignerScene.h"
#include "Designer/IO/DesignerSceneWriter.h"
#include "Designer/IO/DesignerSceneReader.h"
#include "App/Misc/EditorSettings.h"
#include "Designer/Imposter/ImposterObject.h"
#include "Designer/Imposter/SpecializedImposterBinder.h"

#include <Common/assertion.h>
#include <Common/logging.h>
#include <Common/profiling.h>
#include <Common/io_exceptions.h>
#include <Utility/Timer.h>
#include <DataIO/FileSystem/Path.h>
#include <DataIO/FileSystem/Filesystem.h>
#include <SDL/TSdl.h>
#include <SDL/SdlSceneFileWriter.h>
#include <SDL/SdlSceneFileReader.h>
#include <SDL/Introspect/SdlOutputContext.h>
#include <SDL/SdlResourceLocator.h>
#include <SDL/sdl_exceptions.h>
#include <SDL/SceneDescription.h>

#include <utility>
#include <memory>
#include <vector>

namespace ph::editor
{

PH_DEFINE_INTERNAL_LOG_GROUP(Editor, App);

namespace
{

inline Path get_editor_settings_file_path()
{
	return get_editor_data_directory() / "Settings.p2";
}

}// end anonymous namespace

Editor::Editor() = default;

Editor::~Editor()
{
	// Make sure everything is cleaned up
	PH_ASSERT_EQ(m_scenes.size(), 0);
	PH_ASSERT_EQ(m_removingScenes.size(), 0);
}

void Editor::start()
{
	// Load editor settings or create one if no saved file was found
	{
		PH_ASSERT(!m_settings);

		Path settingsFile = get_editor_settings_file_path();
		if(Filesystem::hasFile(settingsFile))
		{
			loadSettings(settingsFile);
		}
		else
		{
			PH_LOG(Editor, Note,
				"No saved editor settings found, creating new one");

			m_settings = TSdl<EditorSettings>::makeResource();
		}
	}

	if(getSettings().loadDefaultSceneOnStartup)
	{
		loadDefaultScene();
	}
}

void Editor::update(const MainThreadUpdateContext& ctx)
{
	PH_PROFILE_SCOPE();

	// Process events
	{
		auto eventFlushTimer = Timer().start();

		flushAllEvents();

		editorStats.mainThreadEventFlushMs = eventFlushTimer.stop().getDeltaMs<float32>();
	}

	for(auto& scene : m_scenes)
	{
		scene->update(ctx);
	}

	cleanupRemovingScenes();
}

void Editor::renderUpdate(const MainThreadRenderUpdateContext& ctx)
{
	PH_PROFILE_SCOPE();

	for(auto& scene : m_scenes)
	{
		scene->renderUpdate(ctx);
	}
}

void Editor::createRenderCommands(RenderThreadCaller& caller)
{
	PH_PROFILE_SCOPE();

	for(auto& scene : m_scenes)
	{
		scene->createRenderCommands(caller);
	}

	renderCleanupRemovingScenes(caller);
}

void Editor::beforeUpdateStage()
{
	PH_PROFILE_SCOPE();

	for(auto& scene : m_scenes)
	{
		scene->beforeUpdateStage();
	}
}

void Editor::afterUpdateStage()
{
	PH_PROFILE_SCOPE();

	for(auto& scene : m_scenes)
	{
		scene->afterUpdateStage();
	}
}

void Editor::beforeRenderStage()
{
	PH_PROFILE_SCOPE();

	for(auto& scene : m_scenes)
	{
		scene->beforeRenderStage();
	}
}

void Editor::afterRenderStage()
{
	PH_PROFILE_SCOPE();

	for(auto& scene : m_scenes)
	{
		scene->afterRenderStage();
	}
}

void Editor::stop()
{
	PH_LOG(Editor, Note,
		"{} scenes to be closed", m_scenes.size());

	// Remove all scenes from back to front
	while(numScenes() != 0)
	{
		const auto sceneIdx = numScenes() - 1;
		removeScene(sceneIdx);
	}

	saveSettings(get_editor_settings_file_path());
}

std::size_t Editor::newScene()
{
	DesignerScene* scene = nullptr;
	auto sceneIndex = nullSceneIndex();

	// Create and add the scene to storage
	{
		auto newScene = std::make_unique<DesignerScene>(TSdl<DesignerScene>::make(this));

		scene = m_scenes.add(std::move(newScene));
		sceneIndex = m_scenes.size() - 1;
	}

	onDesignerSceneAdded.dispatch(DesignerSceneAddedEvent(scene, this));

	return sceneIndex;
}

void Editor::loadDefaultScene()
{
	if(!getSettings().defaultSceneFile.isEmpty())
	{
		loadScene(getSettings().defaultSceneFile);
	}
	else
	{
		// TODO: from a startup scene template (file) if the one from settings is invalid
		//createScene();
	}
}

void Editor::cleanupRemovingScenes()
{
	auto removingScene = m_removingScenes.begin();
	while(removingScene != m_removingScenes.end())
	{
		// Perform cleanup
		if(removingScene->hasRenderCleanupDone && !removingScene->hasCleanupDone)
		{
			removingScene->scene->cleanup();
			removingScene->hasCleanupDone = true;
		}

		// Remove from removing list if fully cleaned up
		if(removingScene->hasRenderCleanupDone && removingScene->hasCleanupDone)
		{
			removingScene = m_removingScenes.erase(removingScene);
		}
		else
		{
			++removingScene;
		}
	}
}

void Editor::renderCleanupRemovingScenes(RenderThreadCaller& caller)
{
	auto removingScene = m_removingScenes.begin();
	while(removingScene != m_removingScenes.end())
	{
		if(!removingScene->hasRenderCleanupDone)
		{
			PH_ASSERT(!removingScene->hasCleanupDone);
			removingScene->scene->renderCleanup(caller);
			removingScene->hasRenderCleanupDone = true;
		}

		++removingScene;
	}
}

void Editor::renderCleanup(RenderThreadCaller& caller)
{
	// All live scenes should have been removed at this point
	PH_ASSERT_EQ(numScenes(), 0);

	renderCleanupRemovingScenes(caller);
}

void Editor::cleanup()
{
	// All live scenes should have been removed at this point
	PH_ASSERT_EQ(numScenes(), 0);

	cleanupRemovingScenes();
}

std::size_t Editor::createScene(const Path& workingDirectory, const std::string& name)
{
	auto sceneIdx = newScene();
	DesignerScene* scene = getScene(sceneIdx);

	setActiveScene(sceneIdx);

	// Fill the created new scene with required initial properties
	
	// Optionally keep the default scene name
	if(!name.empty())
	{
		scene->setName(name);
	}

	// The description link will be empty if the designer scene is a newly created one. 
	// Set the link to the same folder and same name as the designer scene (bundled description).
	PH_ASSERT(!scene->getRenderDescriptionLink().hasIdentifier());
	scene->setRenderDescriptionLink(SdlResourceLocator(SdlOutputContext(&workingDirectory))
		.toBundleIdentifier(workingDirectory / (scene->getName() + ".p2")));

	// Bundled description uses the same working directory as the designer scene
	scene->setWorkingDirectory(workingDirectory);
	scene->getRenderDescription().setWorkingDirectory(workingDirectory);

	PH_LOG(Editor, Note,
		"created scene \"{}\"", scene->getName());

	// Save the scene once, so any necessary files and directories can be created
	saveScene();

	return sceneIdx;
}

std::size_t Editor::createSceneFromDescription(
	const Path& workingDirectory,
	const Path& sceneDescription,
	const std::string& name,
	const std::string& descName)
{
	if(sceneDescription.isEmpty())
	{
		PH_LOG(Editor, Warning,
			"Creating scene \"{}\" from description, however the description is empty. Resorting to "
			"creating a scene normally.", name);
		return createScene(workingDirectory, name);
	}

	Path bundledDesc;
	try
	{
		if(Filesystem::hasFile(sceneDescription))
		{
			if(!descName.empty())
			{
				PH_LOG(Editor, Note,
					"Ignoring input description name \"{}\" as this is redundant for single-file "
					"description.", descName);
			}

			Filesystem::copyFileToDirectory(sceneDescription, workingDirectory, true);
			bundledDesc = workingDirectory / sceneDescription.getFilename();
		}
		else if(Filesystem::hasDirectory(sceneDescription))
		{
			Filesystem::copy(sceneDescription, workingDirectory, true);

			std::string descFilename = descName + ".p2";
			if(descName.empty())
			{
				PH_LOG(Editor, Warning,
					"Description name is required for directory-based description. "
					"Defaulting to \"{}\"", name);

				descFilename = name + ".p2";
			}

			bundledDesc = workingDirectory / descFilename;
		}
	}
	catch(const FilesystemError& e)
	{
		PH_LOG(Editor, Error,
			"Cannot create scene \"{}\" from description: {}.", name, e.whatStr());
	}

	if(!Filesystem::hasFile(bundledDesc))
	{
		PH_LOG(Editor, Error,
			"Bundled scene description not found on path \"{}\".", bundledDesc);
		return nullSceneIndex();
	}
	
	auto sceneIdx = newScene();
	DesignerScene* scene = getScene(sceneIdx);

	setActiveScene(sceneIdx);

	// Fill the created new scene with required initial properties
	
	// Optionally keep the default scene name
	if(!name.empty())
	{
		scene->setName(name);
	}

	// The description link will be empty if the designer scene is a newly created one. 
	// Set the link to the same folder and same name as the designer scene (bundled description).
	PH_ASSERT(!scene->getRenderDescriptionLink().hasIdentifier());
	scene->setRenderDescriptionLink(SdlResourceLocator(SdlOutputContext(&workingDirectory))
		.toBundleIdentifier(bundledDesc));

	// Bundled description uses the same working directory as the designer scene
	scene->setWorkingDirectory(workingDirectory);
	scene->getRenderDescription().setWorkingDirectory(workingDirectory);

	// Read render description
	try
	{
		SdlSceneFileReader reader(bundledDesc, workingDirectory);
		reader.read(&(scene->getRenderDescription()));
	}
	catch(const Exception& e)
	{
		PH_LOG(Editor, Error,
			"Scene description loading failed: {}", e.what());
	}

	// Once description is loaded, create imposters for each of the contained description resource.
	{
		SceneDescription& desc = scene->getRenderDescription();

		std::vector<std::string> resourceNames;
		desc.getResources().getAll(&resourceNames);
		for(std::size_t ri = 0; ri < resourceNames.size(); ++ri)
		{
			SpecializedImposterBinder binder(*scene);
			ImposterObject* imposter = binder.newImposter(resourceNames[ri]);
			if(imposter)
			{
				// We know the name is unique since resource name is unique and the designer scene
				// contains no object initially.
				imposter->setUniqueName(resourceNames[ri]);
			}
		}

		PH_LOG(Editor, Note,
			"Generated {} imposter bindings in scene \"{}\"", resourceNames.size(), scene->getName());
	}

	PH_LOG(Editor, Note,
		"created scene \"{}\"", scene->getName());

	// Save the scene once, so any necessary files and directories can be created
	saveScene();

	return sceneIdx;
}

std::size_t Editor::loadScene(const Path& sceneFile)
{
	// Firstly, make sure the designer scene is there

	const std::string& filenameExt = sceneFile.getExtension();
	if(filenameExt != ".pds")
	{
		PH_LOG(Editor, Error,
			"Cannot load designer scene \"{}\": unsupported file type.", sceneFile);
		return nullSceneIndex();
	}
	
	if(!Filesystem::hasFile(sceneFile))
	{
		PH_LOG(Editor, Error,
			"Cannot load designer scene \"{}\": file does not exist.", sceneFile);
		return nullSceneIndex();
	}

	// Make new scene then load data into it
	auto sceneIdx = newScene();
	DesignerScene* scene = getScene(sceneIdx);

	// Read designer scene
	try
	{
		const Path& workingDirectory = sceneFile.getParent();
		const std::string& sceneName = sceneFile.removeExtension().getFilename();

		DesignerSceneReader reader(workingDirectory);
		reader.setSceneInfo(sceneName, scene);
		reader.read();
	}
	catch(const Exception& e)
	{
		PH_LOG(Editor, Error,
			"Designer scene loading failed: {}", e.what());
	}
	
	// Read render description
	try
	{
		if(scene->getRenderDescriptionLink().isResolved())
		{
			const Path& descFile = scene->getRenderDescriptionLink().getPath();
			const Path& workingDirectory = sceneFile.getParent();

			SdlSceneFileReader reader(descFile, workingDirectory);
			reader.read(&(scene->getRenderDescription()));
		}
		else
		{
			throw_formatted<SdlLoadError>(
				"Cannot load render description of designer scene \"{}\": "
				"description link ({}) is unresolved.",
				scene->getName(), scene->getRenderDescriptionLink());
		}
	}
	catch(const Exception& e)
	{
		PH_LOG(Editor, Error,
			"Scene description loading failed: {}", e.what());
	}

	// Bind descriptions for all imposters once designer scene & description are both loaded
	// (so we have enough information to bind them)
	{
		SceneDescription& desc = scene->getRenderDescription();

		std::vector<ImposterObject*> imposters;
		scene->findObjectsByType(imposters);

		for(ImposterObject* imposter : imposters)
		{
			const auto& descName = imposter->getDescriptionName();
			imposter->bindDescription(desc.get(descName), descName);
		}

		PH_LOG(Editor, Note,
			"Processed {} imposter bindings in scene \"{}\"", imposters.size(), scene->getName());
	}

	setActiveScene(sceneIdx);
	return sceneIdx;
}

void Editor::saveScene()
{
	if(!m_activeScene)
	{
		PH_LOG(Editor, Warning,
			"Cannot save scene--current active scene is null.");
		return;
	}

	saveScene(getSceneIndex(m_activeScene));
}

void Editor::saveScene(std::size_t sceneIndex)
{
	DesignerScene* scene = getScene(sceneIndex);
	if(!scene)
	{
		PH_LOG(Editor, Warning,
			"Cannot save scene--input scene is null (scene index: {}).", sceneIndex);
		return;
	}

	if(scene == m_activeScene)
	{
		scene->pause();
	}

	// Save designer scene
	try
	{
		DesignerSceneWriter writer;
		if(scene->getWorkingDirectory().isEmpty())
		{
			PH_LOG(Editor, Warning,
				"Designer scene has no working directory specified, using writer's: {}",
				writer.getSceneWorkingDirectory());
		}
		else
		{
			// Obey the working directory from designer scene
			writer.setSceneWorkingDirectory(scene->getWorkingDirectory());
		}

		writer.write(*scene);
	}
	catch(const Exception& e)
	{
		PH_LOG(Editor, Error,
			"Designer scene saving failed: {}", e.what());
	}

	// Save render description
	try
	{
		const SceneDescription& description = scene->getRenderDescription();

		SdlSceneFileWriter writer;
		if(description.getWorkingDirectory().isEmpty())
		{
			PH_LOG(Editor, Warning,
				"Render description has no working directory specified, using writer's: {}",
				writer.getSceneWorkingDirectory());
		}
		else
		{
			// Obey the working directory from scene description
			writer.setSceneWorkingDirectory(description.getWorkingDirectory());
		}

		// Extract description file path using link from the designer scene
		const ResourceIdentifier& descLink = scene->getRenderDescriptionLink();
		PH_ASSERT(descLink.isResolved());
		writer.setSceneFile(descLink.getPath());

		writer.write(scene->getRenderDescription());
	}
	catch(const Exception& e)
	{
		PH_LOG(Editor, Error,
			"Scene description saving failed: {}", e.what());
	}
	
	if(scene == m_activeScene)
	{
		scene->resume();
	}
}

void Editor::setActiveScene(const std::size_t sceneIndex)
{
	DesignerScene* sceneToBeActive = getScene(sceneIndex);
	if(sceneToBeActive != m_activeScene)
	{
		DesignerScene* oldActiveScene = m_activeScene;
		DesignerScene* newActiveScene = sceneToBeActive;

		if(oldActiveScene)
		{
			oldActiveScene->pause();
		}

		if(newActiveScene)
		{
			newActiveScene->resume();
		}

		m_activeScene = newActiveScene;
		if(m_activeScene)
		{
			PH_LOG(Editor, Note,
				"scene \"{}\" is now active",
				m_activeScene->getName());
		}
		else
		{
			PH_LOG(Editor, Note,
				"no scene is now active");
		}

		onActiveDesignerSceneChanged.dispatch(
			ActiveDesignerSceneChangedEvent(newActiveScene, oldActiveScene, this));
		postEvent(
			EditContextUpdatedEvent(EEditContextEvent::ActiveSceneChanged, this), onEditContextUpdated);
	}
}

void Editor::removeScene(const std::size_t sceneIndex)
{
	if(sceneIndex >= m_scenes.size())
	{
		PH_LOG(Editor, Warning,
			"unable to remove scene (scene index {} is invalid, must < {})",
			sceneIndex, m_scenes.size());
		return;
	}

	// TODO: ask whether to save current scene

	// Reassign another scene as the active one before removal
	if(m_scenes.size() == 1)
	{
		// The scene to be removed is the last one, set active one to null
		setActiveScene(nullSceneIndex());
	}
	else
	{
		PH_ASSERT_GE(m_scenes.size(), 2);
		for(std::size_t i = 0; i < m_scenes.size(); ++i)
		{
			if(i != sceneIndex)
			{
				setActiveScene(i);
				break;
			}
		}
	}

	m_removingScenes.push_back({
		.scene = m_scenes.remove(sceneIndex),
		.hasRenderCleanupDone = false,
		.hasCleanupDone = false});

	PH_LOG(Editor, Note,
		"removed scene \"{}\"", 
		m_removingScenes.back().scene->getName());

	onDesignerSceneRemoval.dispatch(DesignerSceneRemovalEvent(m_removingScenes.back().scene.get(), this));
}

void Editor::loadSettings(const Path& settingsFile)
{
	if(!Filesystem::hasFile(settingsFile))
	{
		PH_LOG(Editor, Warning,
			"Failed to load editor settings {} (file does not exist)",
			settingsFile);
		return;
	}

	PH_LOG(Editor, Note,
		"Loading editor settings {}",
		settingsFile);

	m_settings = TSdl<EditorSettings>::loadResource(settingsFile);
}

void Editor::saveSettings(const Path& settingsFile) const
{
	if(!m_settings)
	{
		PH_LOG(Editor, Warning,
			"Failed to save editor settings {} (no data present)",
			settingsFile);
		return;
	}

	PH_LOG(Editor, Note,
		"Saving editor settings {}",
		settingsFile);

	PH_ASSERT(m_settings);
	TSdl<EditorSettings>::saveResource(m_settings, settingsFile);
}

EditorSettings& Editor::getSettings() const
{
	PH_ASSERT(m_settings);

	return *m_settings;
}

void Editor::flushAllEvents()
{
	PH_ASSERT(Threads::isOnMainThread());

	// Dispatch queued events to listeners
	m_eventPostQueue.flushAllEvents();
}

EditContext Editor::getEditContext() const
{
	EditContext ctx;
	ctx.activeScene = m_activeScene;
	return ctx;
}

}// end namespace ph::editor
