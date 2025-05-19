#pragma once

#include "Engine/Actor/Material/SurfaceMaterial.h"
#include "Engine/Math/math_fwd.h"
#include "Engine/Actor/Image/Image.h"
#include "Engine/DataIO/FileSystem/Path.h"

#include <memory>
#include <vector>

namespace ph
{

class ThinFilm : public SurfaceMaterial
{
public:
	ThinFilm();

	void storeCooked(
		CookedMaterial& out_material,
		const CookingContext& ctx) const override;

private:
	std::vector<real> m_wavelengthTable;
	std::vector<real> m_reflectanceTable;
	std::vector<real> m_transmittanceTable;
};

}// end namespace ph
