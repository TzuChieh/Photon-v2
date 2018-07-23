#pragma once

#include "Core/Quantity/SpectralStrength.h"
#include "Common/primitive_type.h"
#include "Core/SurfaceBehavior/SurfaceOptics/LaurentBelcour/LbLayer.h"
#include "Core/SurfaceBehavior/SurfaceOptics/LaurentBelcour/TableFGD.h"
#include "Core/SurfaceBehavior/SurfaceOptics/LaurentBelcour/TableTIR.h"
#include "FileIO/FileSystem/CoreResource.h"
#include "Common/assertion.h"

namespace ph
{

class InterfaceStatistics
{
public:
	InterfaceStatistics(real cosWi, const LbLayer& layer1);

	bool addLayer(const LbLayer& layer2);

	real getEquivalentAlpha() const;
	const SpectralStrength& getEnergyScale() const;

private:
	real             m_equivalentAlpha;
	SpectralStrength m_energyScale;

	LbLayer m_layer0, m_layer1;

	real             m_cosWi;
	SpectralStrength m_R0i,  m_Ri0,  m_T0i,  m_Ti0;
	real             m_sR0i, m_sRi0, m_sT0i, m_sTi0;
	real             m_J0i,  m_Ji0;

	static const TableFGD& FGD();
	static const TableTIR& TIR();
};

// In-header Implementations:

inline InterfaceStatistics::InterfaceStatistics(const real cosWi, const LbLayer& layer1) :

	m_equivalentAlpha(0.0_r), 
	m_energyScale(1.0_r),

	m_layer0(layer1), m_layer1(layer1),

	m_cosWi(cosWi),
	m_R0i (0), m_Ri0 (0), m_T0i (1), m_Ti0 (1),
	m_sR0i(0), m_sRi0(0), m_sT0i(0), m_sTi0(0),
	m_J0i (1), m_Ji0 (1)
{
	PH_ASSERT(0.0_r <= m_cosWi && m_cosWi <= 1.0_r);

	// DEBUG
	const TableFGD& fgd = FGD();
	const TableTIR& tir = TIR();
}

inline real InterfaceStatistics::getEquivalentAlpha() const
{
	return m_equivalentAlpha;
}

inline const SpectralStrength& InterfaceStatistics::getEnergyScale() const
{
	return m_energyScale;
}

inline const TableFGD& InterfaceStatistics::FGD()
{
	static const TableFGD table(CoreResource("LaurentBelcourBsdf/table_FGD.bin").getPath());
	return table;
}

inline const TableTIR& InterfaceStatistics::TIR()
{
	static const TableTIR table(CoreResource("LaurentBelcourBsdf/table_TIR.bin").getPath());
	return table;
}

}// end namespace ph