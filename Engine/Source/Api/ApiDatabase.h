#pragma once

#include "Utility/TStableIndexDenseArray.h"

#include <memory>

namespace ph
{

class HDRFrame;
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

	static std::size_t addHdrFrame(std::unique_ptr<HDRFrame> hdrFrame);
	static bool removeHdrFrame(const std::size_t frameId);
	static HDRFrame* getHdrFrame(const std::size_t frameId);

	static std::size_t addWrold(std::unique_ptr<World> world);
	static bool removeWrold(const std::size_t worldId);
	static World* getWrold(const std::size_t worldId);

	static std::size_t addCamera(std::unique_ptr<Camera> camera);
	static bool removeCamera(const std::size_t cameraId);
	static Camera* getCamera(const std::size_t cameraId);

	static void releaseAllData();

private:
	static TStableIndexDenseArray<std::unique_ptr<RenderTask>> renderTasks;
	static TStableIndexDenseArray<std::unique_ptr<Renderer>>   renderers;
	static TStableIndexDenseArray<std::unique_ptr<HDRFrame>>   hdrFrames;
	static TStableIndexDenseArray<std::unique_ptr<World>>      worlds;
	static TStableIndexDenseArray<std::unique_ptr<Camera>>     cameras;
};

}// end namespace ph