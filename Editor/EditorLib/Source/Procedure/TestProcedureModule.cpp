#include "Procedure/TestProcedureModule.h"
#include "App/Module/MainThreadUpdateContext.h"

#include <Common/logging.h>
#include <Actor/Image/RasterFileImage.h>
#include <DataIO/SDL/TSdl.h>

namespace ph::editor
{

PH_DEFINE_INTERNAL_LOG_GROUP(Test, Module);

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
		PH_LOG(Test, "res created");
	}

	res->setFilePath(Path("./rendered_scene - Copy (2).png"));
	RegularPicture pic = res->loadRegularPicture();
	PH_LOG(Test, "pic res: {}", pic.frame.getSizePx());

	EImageWrapMode hWrapMode = res->getHorizontalWrapMode();
	EImageWrapMode vWrapMode = res->getVerticalWrapMode();
	PH_LOG(Test, "pic h warp mode: {}", TSdlEnum<EImageWrapMode>()[hWrapMode]);
	PH_LOG(Test, "pic v warp mode: {}", TSdlEnum<EImageWrapMode>()[vWrapMode]);

	PH_LOG(Test, "pic sample mode: {}", TSdlEnum<EImageSampleMode>()[res->getSampleMode()]);

	// TODO
}

void TestProcedureModule::onDetach()
{
	// TODO
}

}// end namespace ph::editor
