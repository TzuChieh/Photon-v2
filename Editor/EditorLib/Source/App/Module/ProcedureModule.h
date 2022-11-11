#pragma once

#include "App/Module/AppModule.h"

namespace ph::editor
{

class ModuleAttachmentInfo;
class MainThreadUpdateContext;

class ProcedureModule : public AppModule
{
public:
	std::string getName() const override = 0;

	virtual void update(const MainThreadUpdateContext& ctx) = 0;

	void onAttach(const ModuleAttachmentInfo& info) override;
	void onDetach() override;

private:
};

}// end namespace ph::editor
