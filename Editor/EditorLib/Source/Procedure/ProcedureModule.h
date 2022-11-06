#pragma once

namespace ph::editor
{

class ModuleAttachmentInfo;
class MainThreadUpdateContext;

class ProcedureModule
{
public:
	virtual ~ProcedureModule();

	virtual void onAttach(const ModuleAttachmentInfo& info) = 0;
	virtual void onDetach() = 0;
	virtual void update(const MainThreadUpdateContext& ctx) = 0;

private:
};

}// end namespace ph::editor
