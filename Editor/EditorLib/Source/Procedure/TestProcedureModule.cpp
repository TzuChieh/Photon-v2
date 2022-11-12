#include "Procedure/TestProcedureModule.h"
#include "App/Module/MainThreadUpdateContext.h"

#include <Common/logging.h>

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
	// TODO
}

void TestProcedureModule::onDetach()
{
	// TODO
}

}// end namespace ph::editor
