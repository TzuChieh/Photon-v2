#pragma once

#include "Actor/Material/SurfaceMaterial.h"
#include "Math/math_fwd.h"
#include "DataIO/SDL/TCommandInterface.h"
#include "Actor/Image/Image.h"
#include "DataIO/FileSystem/Path.h"

#include <memory>
#include <vector>

namespace ph
{

class ThinFilm : public SurfaceMaterial, public TCommandInterface<ThinFilm>
{
public:
	ThinFilm();

	void genSurface(CookingContext& context, SurfaceBehavior& behavior) const override;

private:
	std::vector<real> m_wavelengthTable;
	std::vector<real> m_reflectanceTable;
	std::vector<real> m_transmittanceTable;

// command interface
public:
	explicit ThinFilm(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
};

}// end namespace ph
