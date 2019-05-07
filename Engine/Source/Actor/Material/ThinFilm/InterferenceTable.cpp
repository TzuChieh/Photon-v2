#include "Actor/Material/ThinFilm/InterferenceTable.h"
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <complex>

namespace ph
{

    typedef std::complex<float> complex_f;
    InterferenceTable::InterferenceTable() = default;
    InterferenceTable::InterferenceTable(const std::vector<complex_f> ior, const std::vector<float> thickness)
    {
        ior_vec = ior;
        thickness_vec = thickness;
    }

    void InterferenceTable::write_to_vec(const float angle, const float wl, const float R, const float T)
    {
        m_incidentAngles.push_back(angle);
        m_wavelengths.push_back(wl);
        m_reflectances.push_back(R);
        m_transmittance.push_back(T);
    }

    void InterferenceTable::read_iorfile(const std::string filename, const int layer)
    {
    	std::string line;
    	std::ifstream myfile(filename);
    	if (myfile.is_open())
    	{
            bool get_n = false;
            bool get_k = false;
    		while (getline(myfile, line))
    		{
                if (line.find("wl,n") != std::string::npos)
                {
                    get_n = true;
            		while (getline(myfile, line))
                    {
                        if (line.length() == 1) break;

            			size_t pos = line.find(",");
            			float wavelength = std::stof(line.substr(0, pos)) * 1000;
            			float n = std::stof(line.substr(pos+1));
            			if (wavelength >= wl_min && wavelength <= wl_max)
            			{
            				ior_file[layer][wavelength] = complex_f(n,0);
            			}
                    }
                }
                if (line.find("wl,k") != std::string::npos)
                {
                    get_k = true;
            		while (getline(myfile, line))
                    {
            			size_t pos = line.find(",");
            			float wavelength = std::stof(line.substr(0, pos)) * 1000;
            			float k = std::stof(line.substr(pos+1));
            			if (wavelength >= wl_min && wavelength <= wl_max)
            			{
            				ior_file[layer][wavelength] += complex_f(0,k*(-1));
            			}
                    }
                }
    		}

            if (_debug)
            {
                if (get_n && get_k) std::cout << "successful init ior_file: " << filename << "\n";
                else std::cout << "failed to init ior_file: " << filename << "\n";
            }
    		myfile.close();
    	}
        else
        {
            if (_debug)
                std::cout << "failed to open ior_file: " << filename << '\n';
        }
    }

    void InterferenceTable::output_log()
    {
		std::ofstream f_log("InterferenceTable.log");
		if (f_log.is_open())
		{
			std::string str_log = ss_log.str();
			f_log << str_log;
			f_log.close();
		}
    }

    void InterferenceTable::output_result()
    {
    	std::string s1 = "incident_angle.txt";
    	std::string s2 = "wavelength.txt";
    	std::string s3 = "reflectance.txt";
    	std::string s4 = "transmittance.txt";

    	std::ofstream f1(s1);
    	std::ofstream f2(s2);
    	std::ofstream f3(s3);
    	std::ofstream f4(s4);

    	if (f1.is_open() && f2.is_open() && f3.is_open() && f4.is_open())
    	{
    		std::stringstream ss1;
    		std::stringstream ss2;
    		std::stringstream ss3;
    		std::stringstream ss4;

            for (int i=0; i<m_reflectances.size(); ++i)
            {
                ss1 << m_incidentAngles[i] << '\n';
                ss2 << m_wavelengths[i] << '\n';
                ss3 << m_reflectances[i] << '\n';
                ss4 << m_transmittance[i] << '\n';
            }

    		std::string o1 = ss1.str();
    		std::string o2 = ss2.str();
    		std::string o3 = ss3.str();
    		std::string o4 = ss4.str();

    		f1 << o1;
    		f2 << o2;
    		f3 << o3;
    		f4 << o4;

    		f1.close();
    		f2.close();
    		f3.close();
    		f4.close();
    	}
    }

    void InterferenceTable::simulate_single_thin_film()
    {
    	complex_f n1 = ior_vec[0];
    	complex_f n2 = ior_vec[1];
    	complex_f n3 = ior_vec[2];
        int thickness = thickness_vec[1];

		for (int theta = 0; theta <= 90; theta += delta_angle)
    	{

            for (int wl = wl_min; wl <= wl_max; wl += delta_wl)
    		{
                if (ior_file[0].size()) n1 = ior_file[0][wl];
                if (ior_file[1].size()) n2 = ior_file[1][wl];
                if (ior_file[2].size()) n3 = ior_file[2][wl];

                complex_f theta_1 = complex_f(theta, 0);
                complex_f product_1 = n1*angle_sin(theta_1);
				complex_f theta_2 = angle_asin(product_1/n2); // n1sin1=n2sin2=n3sin3
                complex_f theta_3 = angle_asin(product_1/n3);
				if (_debug)
                {
                    ss_log << "n1: " << n1 << " n2: " << n2 << " n3: " << n3 << '\n';
                    ss_log << "theta_1:" << theta_1 << ", theta_2:" << theta_2 << ", theta_3:" << theta_3 << " wl:" << wl << '\n';
                }

                std::vector<float> RT = calc_recur_R(n1, n2, n3, theta_1, theta_2, theta_3, wl, thickness);

                write_to_vec(theta, wl, RT[0], RT[1]);
    		}

    	}

    }

    std::vector<float> InterferenceTable::calc_recur_R(const complex_f n1, const complex_f n2, const complex_f n3, const complex_f theta_1, const complex_f theta_2, const complex_f theta_3, const int wl, const int d)
    {
    	complex_f rs1 = calc_rs(n1, n2, theta_1, theta_2);
    	complex_f rp1 = calc_rp(n1, n2, theta_1, theta_2);
    	complex_f rs2 = calc_rs(n2, n3, theta_2, theta_3);
    	complex_f rp2 = calc_rp(n2, n3, theta_2, theta_3);

    	if(_debug) ss_log << "rs1:" << rs1 << ", rp1:" << rp1 << ", rs2:" << rs2 << ", rp2:" << rp2 << "\n";

        complex_f one(1,0);
        complex_f i(0,1);
    	// complex_f ts1 = rs1+one;
    	// complex_f tp1 = (rp1+one)*n1/n2;
    	// complex_f ts2 = rs2+one;
    	// complex_f tp2 = (rp2+one)*n2/n1;

    	// if(_debug) ss_log << "ts1:" << ts1 << ", tp1:" << tp1 << ", ts2:" << ts2 << ", tp2:" << tp2 << "\n";

    	complex_f phi = float(4*(PI/wl)*d)*n2*angle_cos(theta_2);
    	complex_f phase_diff = std::exp(float(-1)*phi*i);
    	complex_f phase_diff_half = std::exp(float(-0.5)*phi*i);
    	complex_f rs_recur = (rs1*one + rs2*phase_diff) / (one+rs1*rs2*phase_diff);
    	complex_f rp_recur = (rp1*one + rp2*phase_diff) / (one+rp1*rp2*phase_diff);
    	// complex_f ts_recur = (ts1*ts2*phase_diff_half) / (one+rs1*rs2*phase_diff);
    	// complex_f tp_recur = (tp1*tp2*phase_diff_half) / (one+rp1*rp2*phase_diff);

    	if(_debug) ss_log << "rs:" << rs_recur << ", rp:" << rp_recur
            // << ", ts:" << ts_recur << ", tp:" << tp_recur
            << "\n";

    	float abs_s = abs(rs_recur);
    	float abs_p = abs(rp_recur);
    	float Reff = (abs_s*abs_s+abs_p*abs_p)/2;

    	// float abs_ts = abs(ts_recur);
    	// float abs_tp = abs(tp_recur);
    	// float Teff = abs((n3/n1)*(angle_cos(theta_3)/angle_cos(theta_1))*(abs_ts*abs_ts+abs_tp*abs_tp))/2;
        float Teff = 0;
        if (n1.imag() == 0 && n2.imag() == 0 && n3.imag() == 0) Teff = 1 - Reff;

    	if(_debug) ss_log << "|rs|:" << abs_s << ", |rp|:" << abs_p << ", Reff:" << Reff << "\n";
    	if(_debug) ss_log
            // << "|ts|:" << abs_ts << ", |tp|:" << abs_tp << ", "
            << "Teff:" << Teff << "\n\n";

        std::vector<float> RT;
        RT.push_back(Reff);
        RT.push_back(Teff);
    	return RT;
    }

    complex_f InterferenceTable::calc_rs(const complex_f n1, const complex_f n2, const complex_f theta_i, const complex_f theta_t)
    {
    	if (theta_i == complex_f(90,0)) return -1;
    	if (theta_t == complex_f(90,0)) return 1;
    	complex_f p1 = n1*angle_cos(theta_i);
    	complex_f p2 = n2*angle_cos(theta_t);
    	return (p1-p2)/(p1+p2);
    }

    complex_f InterferenceTable::calc_rp(const complex_f n1, const complex_f n2, const complex_f theta_i, const complex_f theta_t)
    {
    	if (theta_i == complex_f(90,0)) return -1;
    	if (theta_t == complex_f(90,0)) return 1;
    	complex_f p1 = n2*angle_cos(theta_i);
    	complex_f p2 = n1*angle_cos(theta_t);
    	return (p1-p2)/(p1+p2);
    }


}// end namespace ph
