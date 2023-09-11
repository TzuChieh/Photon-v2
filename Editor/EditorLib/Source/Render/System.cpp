#include "Render/System.h"
#include "editor_lib_config.h"

#include <Common/logging.h>

#include <utility>
#include <memory>

namespace ph::editor::render
{

PH_DEFINE_INTERNAL_LOG_GROUP(System, Render);

namespace
{

inline void run_single_file_reading_work(const System::FileReadingWork& work)
{
	if(work)
	{
		work();
	}
	else
	{
		PH_LOG_WARNING(System,
			"One file reading work is empty, skipping.");
	}
}

}// end anonymous namespace

System::System()
	: updateCtx()
	, scenes()

	, m_mainScene(nullptr)
	, m_graphicsCtx(nullptr)

	, m_fileReadingThread()
{
	m_fileReadingThread.setWorkProcessor(
		[](const FileReadingWork& work)
		{
			run_single_file_reading_work(work);
		});
	m_fileReadingThread.start();

	m_mainScene = scenes.add(std::make_unique<Scene>(*this));
}

System::~System() = default;

void System::addFileReadingWork(FileReadingWork work)
{
	if constexpr(config::ENABLE_RENDER_SYSTEM_ASYNC_FILE_READING)
	{
		m_fileReadingThread.addWork(std::move(work));
	}
	else
	{
		run_single_file_reading_work(work);
	}
}

void System::waitAllFileReadingWorks()
{
	if constexpr(config::ENABLE_RENDER_SYSTEM_ASYNC_FILE_READING)
	{
		m_fileReadingThread.waitAllWorks();
	}
}

void System::setGraphicsContext(GraphicsContext* ctx)
{
	if(m_graphicsCtx == ctx)
	{
		return;
	}

	// Must wait all concurrent works to finish, they may be using the context
	waitAllFileReadingWorks();

	m_graphicsCtx = ctx;
}

}// end namespace ph::editor::render
