#pragma once

#include <Utility/TSpan.h>

#include <string>
#include <vector>

namespace ph { class SdlClass; }

namespace ph::editor
{

class ImguiEditorUIProxy;

class ImguiEditorObjectTypeMenu final
{
public:
	ImguiEditorObjectTypeMenu();

	void buildMenuButton(
		const char* name,
		const SdlClass*& out_selectedClass);

private:
	struct ObjectType
	{
		const SdlClass* clazz = nullptr;
		std::string displayName;
	};

	static std::vector<ObjectType> gatherObjectTypes();
	static TSpanView<ObjectType> getObjectTypes();
};

}// end namespace ph::editor
