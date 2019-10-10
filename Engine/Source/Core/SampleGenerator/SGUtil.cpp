#include "Core/SampleGenerator/SGUtil.h"
#include "Math/Random.h"

namespace ph
{

void SGUtil::genArray2dStrata(math::Vector2R* const out_coordArray2Ds,
                              const std::size_t numXs, const std::size_t numYs)
{
	const real dx = 1.0_r / static_cast<real>(numXs);
	const real dy = 1.0_r / static_cast<real>(numYs);
	for(std::size_t y = 0; y < numYs; y++)
	{
		const std::size_t baseIndex = y * numYs;
		for(std::size_t x = 0; x < numXs; x++)
		{
			const real jitterX = math::Random::genUniformReal_i0_e1();
			const real jitterY = math::Random::genUniformReal_i0_e1();
			out_coordArray2Ds[baseIndex + x].x = (static_cast<real>(x) + jitterX) * dx;
			out_coordArray2Ds[baseIndex + x].y = (static_cast<real>(y) + jitterY) * dy;
		}
	}
}

void SGUtil::genArray2dLatinHypercube(math::Vector2R* const out_coordArray2Ds,
                                      const std::size_t num2Ds)
{
	// generate jittered 2D samples along area diagonal
	const real reciNum2Ds = 1.0_r / static_cast<real>(num2Ds);
	for(std::size_t ii = 0; ii < num2Ds; ii++)
	{
		const real jitterX = math::Random::genUniformReal_i0_e1();
		const real jitterY = math::Random::genUniformReal_i0_e1();
		out_coordArray2Ds[ii].x = (static_cast<real>(ii) + jitterX) * reciNum2Ds;
		out_coordArray2Ds[ii].y = (static_cast<real>(ii) + jitterY) * reciNum2Ds;
	}

	// TODO: shuffle
	PH_ASSERT_UNREACHABLE_SECTION();
}

}// end namespace ph
