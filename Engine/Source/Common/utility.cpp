#include "Common/utility.h"

#include <iostream>

namespace ph::detail
{

void output_not_implemented_warning(
	const std::string& filename,
	const std::string& lineNumber)
{
	std::cerr << "code not implemented in <" << filename << ">: "
	          << "line " << lineNumber << std::endl;
}

}// end namespace ph::detail
