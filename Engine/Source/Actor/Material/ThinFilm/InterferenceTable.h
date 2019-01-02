#pragma once

#include <vector>
#include <sstream>
#include <math.h>

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
	InterferenceTable(const std::vector<float>, const std::vector<float>);

	// TODO
	void simulate_single_thin_film();
	float calc_recur_R(const float, const float, const float, const float, const float, const float, const int, const int);
	float calc_rs(const float, const float, const float, const float);
	float calc_rp(const float, const float, const float, const float);
	void write_to_vec(const float, const float, const float, const float);
	void output_result();
	void output_log();

	// HACK
	std::vector<float> getWavelengthsNm() const
	{
		return m_wavelengths;
	}

	std::vector<float> getReflectances() const
	{
		return m_reflectances;
	}

	std::vector<float> getTransmittance() const
	{
		return m_transmittance;
	}

private:
	std::vector<float> m_incidentAngles;
	std::vector<float> m_wavelengths;
	std::vector<float> m_reflectances;
	std::vector<float> m_transmittance;

	std::vector<float> ior_vec;
	std::vector<float> thickness_vec;
	const int wl_min = 400;
	const int wl_max = 700;
	const int delta_wl = 10;
	const int delta_angle = 1;

	float angle_sin(float t) { return sin(a2r(t)); }
	float angle_cos(float t) { return cos(a2r(t)); }
	float angle_tan(float t) { return tan(a2r(t)); }
	float angle_asin(float n) { return r2a(asin(n)); }
	float angle_acos(float n) { return r2a(acos(n)); }
	float a2r(float a) { return a*PI/180; } // angle to rad
	float r2a(float r) { return r*180/PI; } // rad to angle
	float PI=acos(-1); //3.14159265;

	bool _debug = false;
	std::stringstream ss_log;
};

}// end namespace ph
