#pragma once

#include "Utility/TStableIndexDenseArray.h"

#include <memory>

namespace ph
{

class Frame;
class RenderTask;
class Renderer;
class World;
class Camera;

class ApiDatabase final
{
public:
	static std::size_t addRenderTask(std::unique_ptr<RenderTask> renderTask);
	static bool removeRenderTask(const std::size_t renderTaskId);
	static RenderTask* getRenderTask(const std::size_t renderTaskId);

	static std::size_t addRenderer(std::unique_ptr<Renderer> renderer);
	static bool removeRenderer(const std::size_t rendererId);
	static Renderer* getRenderer(const std::size_t rendererId);

	static std::size_t addFrame(std::unique_ptr<Frame> frame);
	static bool removeFrame(const std::size_t frameId);
	static Frame* getFrame(const std::size_t frameId);

	static std::size_t addWorld(std::unique_ptr<World> world);
	static bool removeWorld(const std::size_t worldId);
	static World* getWorld(const std::size_t worldId);

	static std::size_t addCamera(std::unique_ptr<Camera> camera);
	static bool removeCamera(const std::size_t cameraId);
	static Camera* getCamera(const std::size_t cameraId);

	static void releaseAllData();

private:
	static TStableIndexDenseArray<std::unique_ptr<RenderTask>> renderTasks;
	static TStableIndexDenseArray<std::unique_ptr<Renderer>>   renderers;
	static TStableIndexDenseArray<std::unique_ptr<Frame>>      frames;
	static TStableIndexDenseArray<std::unique_ptr<World>>      worlds;
	static TStableIndexDenseArray<std::unique_ptr<Camera>>     cameras;
};

}// end namespace ph