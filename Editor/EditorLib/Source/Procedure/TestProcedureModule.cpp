#include "Procedure/TestProcedureModule.h"
#include "App/Module/MainThreadUpdateContext.h"

#include <Common/logging.h>
#include <Actor/Image/RasterFileImage.h>
#include <Frame/RegularPicture.h>

namespace ph::editor
{

void TestProcedureModule::update(const MainThreadUpdateContext& ctx)
{
	/*PH_LOG(Test,
		"updated, delta = {} sec, frame number = {}", 
		ctx.deltaS, ctx.frameNumber);*/
}

void TestProcedureModule::onAttach(const ModuleAttachmentInfo& info)
{
	auto res = TSdl<RasterFileImage>::makeResource();
	if(res)
	{
		PH_DEFAULT_LOG("res created");
	}

	res->setFilePath(Path("./rendered_scene.png"));
	RegularPicture pic = res->loadRegularPicture();
	PH_DEFAULT_LOG("pic res: {}", pic.getSizePx());

	EImageWrapMode hWrapMode = res->getHorizontalWrapMode();
	EImageWrapMode vWrapMode = res->getVerticalWrapMode();
	PH_DEFAULT_LOG("pic h warp mode: {}", TSdlEnum<EImageWrapMode>()[hWrapMode]);
	PH_DEFAULT_LOG("pic v warp mode: {}", TSdlEnum<EImageWrapMode>()[vWrapMode]);

	PH_DEFAULT_LOG("pic sample mode: {}", TSdlEnum<EImageSampleMode>()[res->getSampleMode()]);

	// TODO
}

void TestProcedureModule::onDetach()
{
	// TODO
}

}// end namespace ph::editor
