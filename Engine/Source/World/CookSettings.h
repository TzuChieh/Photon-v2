#pragma once

#include "DataIO/Option.h"

namespace ph
{

enum class EAccelerator
{
	BRUTE_FORCE,
	BVH,
	KDTREE,
	INDEXED_KDTREE
};

class CookSettings final : public Option
{
public:
	CookSettings();
	explicit CookSettings(EAccelerator topLevelAccelerator);

	void setTopLevelAccelerator(EAccelerator accelerator);
	EAccelerator getTopLevelAccelerator() const;

private:
	EAccelerator m_topLevelAccelerator;

// command interface
public:
	explicit CookSettings(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
};

// In-header Implementations:

inline void CookSettings::setTopLevelAccelerator(const EAccelerator accelerator)
{
	m_topLevelAccelerator = accelerator;
}

inline EAccelerator CookSettings::getTopLevelAccelerator() const
{
	return m_topLevelAccelerator;
}

}// end namespace ph

/*
	<SDL_interface>

	<category>  option        </category>
	<type_name> cook-settings </type_name>

	<name> Cook Settings </name>
	<description>
		Settings related to the actor-cooking process.
	</description>

	<command type="creator">
		<input name="top-level-accelerator" type="string">
			<description>
				Type of the top-level accelerator. Possible values: 
				brute-force, bvh, kd-tree, indexed-kd-tree.
			</description>
		</input>
	</command>

	</SDL_interface>
*/
