#pragma once

#include "Engine/Math/Function/TMathFunction2D.h"
#include "Engine/Math/Geometry/TAABB2D.h"
#include "Engine/Math/TVector2.h"

#include <Common/assertion.h>
#include <Common/primitive_type.h>

#include <algorithm>
#include <array>
#include <cstddef>

namespace ph::math
{

template<typename Value, std::size_t Nx, std::size_t Ny>
class TTabulatedMathFunction2D : public TMathFunction2D<Value>
{
public:
	template<typename SourceFunction>
	TTabulatedMathFunction2D(
		const SourceFunction& sourceFunction,
		const TAABB2D<Value>& domain);

	Value evaluate(Value x, Value y) const override;

private:
	std::array<std::array<float32, Nx>, Ny> m_values;
	TAABB2D<Value> m_domain;
	TVector2<Value> m_rcpCellSize;
};

template<typename Value, std::size_t Nx, std::size_t Ny>
template<typename SourceFunction>
inline TTabulatedMathFunction2D<Value, Nx, Ny>::TTabulatedMathFunction2D(
	const SourceFunction& sourceFunction,
	const TAABB2D<Value>& domain)

	: m_values()
	, m_domain(domain)
	, m_rcpCellSize(
		static_cast<Value>(Nx) / domain.getWidth(),
		static_cast<Value>(Ny) / domain.getHeight())
{
	static_assert(Nx > 0 && Ny > 0);
	PH_ASSERT(domain.isArea());

	const Value cellSizeX = domain.getWidth() / static_cast<Value>(Nx);
	const Value cellSizeY = domain.getHeight() / static_cast<Value>(Ny);
	for(std::size_t y = 0; y < Ny; ++y)
	{
		for(std::size_t x = 0; x < Nx; ++x)
		{
			const Value sampleX = m_domain.getMinVertex().x() +
				(static_cast<Value>(x) + static_cast<Value>(0.5)) * cellSizeX;
			const Value sampleY = m_domain.getMinVertex().y() +
				(static_cast<Value>(y) + static_cast<Value>(0.5)) * cellSizeY;
			m_values[y][x] = static_cast<float32>(sourceFunction.evaluate(sampleX, sampleY));
		}
	}
}

template<typename Value, std::size_t Nx, std::size_t Ny>
inline Value TTabulatedMathFunction2D<Value, Nx, Ny>::evaluate(const Value x, const Value y) const
{
	const auto& min = m_domain.getMinVertex();
	const auto& max = m_domain.getMaxVertex();
	if(x < min.x() || x > max.x() || y < min.y() || y > max.y())
	{
		return static_cast<Value>(0);
	}

	const std::size_t ix = std::min(
		static_cast<std::size_t>((x - min.x()) * m_rcpCellSize.x()),
		Nx - 1);
	const std::size_t iy = std::min(
		static_cast<std::size_t>((y - min.y()) * m_rcpCellSize.y()),
		Ny - 1);

	// Uses nearest-cell lookup; no interpolation or normalization is applied.
	return static_cast<Value>(m_values[iy][ix]);
}

}// end namespace ph::math
