#include "App/HelpMenu/AboutInformation.h"

namespace ph::editor
{

AboutInformation::AboutInformation()
{
	authorInfo = "Developer: Tzu-Chieh Chang";

	citationInfo = 
		"If you use Photon in your research project, you are hightly encouraged to cite it using "
		"the following BibTeX template: \n"
		"\n"
		"@misc{Photon, \n"
		"      Author = {Tzu-Chieh Chang}, \n"
		"      Year   = {2016--2022}, \n"
		"      Title  = {Photon renderer}, \n"
		"      Note   = {https://github.com/TzuChieh/Photon-v2} \n"
		"}";
}

}// end namespace ph::editor
