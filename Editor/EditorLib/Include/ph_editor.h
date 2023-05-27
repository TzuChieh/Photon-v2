#pragma once

#include <vector>

namespace ph { class SdlClass; }
namespace ph { class Path; }

namespace ph::editor
{

/*! @brief Main function for the editor application.
*/
int application_entry_point(int argc, char* argv[]);

/*! @brief Main function for the UI library demo. Useful for playing with all UI features.
*/
int imgui_demo_entry_point(int argc, char* argv[]);

/*! @brief Get a list of registered SDL classes from the editor.
*/
std::vector<const SdlClass*> get_registered_editor_classes();

Path get_editor_data_directory();

}// end namespace ph::editor
