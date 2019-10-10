#pragma once

#include "FileIO/SDL/TCommandInterface.h"

namespace ph
{

enum class EAccelerator
{
	BRUTE_FORCE,
	BVH,
	KDTREE,
	INDEXED_KDTREE
};

class CookSettings : public TCommandInterface<CookSettings>
{
public:
	CookSettings();
	CookSettings(EAccelerator topLevelAccelerator);

	void setTopLevelAccelerator(EAccelerator accelerator);
	EAccelerator getTopLevelAccelerator() const;

private:
	EAccelerator m_topLevelAccelerator;

// command interface
public:
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
