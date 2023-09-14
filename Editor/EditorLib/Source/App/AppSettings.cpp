#include "App/AppSettings.h"

namespace ph::editor
{

TSpanView<const char*> AppSettings::getCmdArgs()
{
	return m_cmdArgs;
}

void AppSettings::setCmdArgs(int argc, char* argv[])
{
	m_cmdArgs = TSpanView<const char*>(argv, argc);
}

}// end namespace ph::editor
