#include <ph_core.h>

namespace
{

using namespace ph;

}// end anonymous namespace

int main(int argc, char* argv[])
{
	if(!init_render_engine(EngineInitSettings{}))
	{
		return EXIT_FAILURE;
	}

	// TODO:

	return exit_render_engine() ? EXIT_SUCCESS : EXIT_FAILURE;
}
