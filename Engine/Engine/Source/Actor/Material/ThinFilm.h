#pragma once

#include "Actor/Material/SurfaceMaterial.h"
#include "Math/math_fwd.h"
#include "Actor/Image/Image.h"
#include "DataIO/FileSystem/Path.h"

#include <memory>
#include <vector>

namespace ph
{

class ThinFilm : public SurfaceMaterial
{
public:
	ThinFilm();

	void genSurface(const CookingContext& ctx, SurfaceBehavior& behavior) const override;

private:
	std::vector<real> m_wavelengthTable;
	std::vector<real> m_reflectanceTable;
	std::vector<real> m_transmittanceTable;
};

}// end namespace ph
