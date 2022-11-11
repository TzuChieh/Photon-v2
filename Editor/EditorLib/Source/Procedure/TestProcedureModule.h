#pragma once

#include "App/Module/ProcedureModule.h"

namespace ph::editor
{

class TestProcedureModule : public ProcedureModule
{
public:
	std::string getName() const override;
	void update(const MainThreadUpdateContext& ctx) override;
	void onAttach(const ModuleAttachmentInfo& info) override;
	void onDetach() override;

private:
};

inline std::string TestProcedureModule::getName() const
{
	return "test";
}

}// end namespace ph::editor
