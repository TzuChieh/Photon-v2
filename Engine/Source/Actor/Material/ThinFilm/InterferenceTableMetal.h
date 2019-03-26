#pragma once

#include <vector>
#include <sstream>
#include <complex>
#include <map>

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
class InterferenceTableMetal
{
public:
	typedef std::complex<float> complex_f;
	InterferenceTableMetal();
	InterferenceTableMetal(const std::vector<complex_f>, const std::vector<float>);

	// TODO
	void simulate_single_thin_film();
	std::vector<float> calc_recur_R(const complex_f, const complex_f, const complex_f, const complex_f, const complex_f, const complex_f, const int, const int);
    complex_f calc_rs(const complex_f, const complex_f, const complex_f, const complex_f);
	complex_f calc_rp(const complex_f, const complex_f, const complex_f, const complex_f);
	void write_to_vec(const float, const float, const float, const float);
	void read_iorfile(const std::string, const int);
	void output_result();
	void output_log();
	void enable_debug() { _debug = true; }

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

	complex_f angle_sin(complex_f t) { return sin(a2r(t)); }
	complex_f angle_cos(complex_f t) { return cos(a2r(t)); }
	complex_f angle_tan(complex_f t) { return tan(a2r(t)); }
	complex_f angle_asin(complex_f n) { return r2a(asin(n)); }
	complex_f angle_acos(complex_f n) { return r2a(acos(n)); }
	complex_f a2r(complex_f a) { return a*(PI/180); } // angle to rad
	complex_f r2a(complex_f r) { return r*(180/PI); } // rad to angle
	float PI = 3.14159265f;
	const int wl_min = 400;
	const int wl_max = 700;
	const int delta_wl = 10;
	const int delta_angle = 1;

private:
	std::vector<float> m_incidentAngles;
	std::vector<float> m_wavelengths;
	std::vector<float> m_reflectances;
	std::vector<float> m_transmittance;

	std::vector<complex_f> ior_vec;
	std::vector<float> thickness_vec;

	std::map<int, std::map<int,complex_f>> ior_file;

	bool _debug = false;
	std::stringstream ss_log;
};

}// end namespace ph
