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

	bool menuButton(
		const char* name,
		const SdlClass*& out_selectedClass,
		bool showImposters = false,
		bool* out_hasSelectedImposter = nullptr);

private:
	struct ObjectType
	{
		const SdlClass* clazz = nullptr;
		std::string displayName;
	};

	static std::vector<ObjectType> gatherGeneralObjectTypes();
	static std::vector<ObjectType> gatherImposterObjectTypes();
	static TSpanView<ObjectType> getGeneralObjectTypes();
	static TSpanView<ObjectType> getImposterObjectTypes();
};

}// end namespace ph::editor
