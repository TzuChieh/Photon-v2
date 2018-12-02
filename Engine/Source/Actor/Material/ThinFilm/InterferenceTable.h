#pragma once

#include <vector>

namespace ph
{

/*
	A table that records spectral reflectance and transmittance for each
	incident angle. 

	Reference:

	Sussenbach, M., "Rendering Iridescent Objects in Real-time", 
	Utrecht University Master Thesis (2013)

	https://dspace.library.uu.nl/handle/1874/287110
*/
class InterferenceTable
{
public:
	InterferenceTable();

	// TODO

private:
	std::vector<float> m_incidentAngles;
	std::vector<float> m_wavelengths;
	std::vector<float> m_reflectances;
	std::vector<float> m_transmittance;
};

}// end namespace ph