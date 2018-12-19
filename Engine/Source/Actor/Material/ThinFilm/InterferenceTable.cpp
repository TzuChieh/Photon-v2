#include "Actor/Material/ThinFilm/InterferenceTable.h"
// #include "InterferenceTable.h"
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <complex>

namespace ph
{
    InterferenceTable::InterferenceTable() = default;
    InterferenceTable::InterferenceTable(const std::vector<float> ior, const std::vector<float> thickness)
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

    void InterferenceTable::output_log()
    {
		std::ofstream f_log("log.txt");
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
    	float n1 = ior_vec[0];
    	float n2 = ior_vec[1];
    	float n3 = ior_vec[2];
        int thickness = thickness_vec[1];

        float crit_angle_1 = 90;
        float crit_angle_2 = 90;
        if (n2 < n1)  crit_angle_1 = angle_asin(n2/n1);
        if (n3 < n1)  crit_angle_2 = angle_asin(n3/n1);
        float crit_angle = std::min(crit_angle_1, crit_angle_2);

    	for (int wl = wl_min; wl <= wl_max; wl += delta_wl)
    	{
    		// float n2 = ior_file[wl];
    		// float crit_angle = 90;
    		// if (n2 < n1)
    		// {
    		//    crit_angle = angle_asin(n2/n1);
    		// }

    		for (int theta_1 = 0; theta_1 <= 90; theta_1 += delta_angle)
    		{
    			float Reff = 1;

    			if (theta_1 <= crit_angle)
    			{
                    float product_1 = n1*angle_sin(theta_1);
    				float theta_2 = angle_asin(product_1/n2); // n1sin1=n2sin2=n3sin3
                    float theta_3 = angle_asin(product_1/n3);
    				if (_debug)
                        ss_log << "theta_1:" << theta_1 << ", theta_2:" << theta_2 << ", theta_3:" << theta_3 << " wl:" << wl << '\n';

                    Reff = calc_recur_R(n1, n2, n3, theta_1, theta_2, theta_3, wl, thickness);
    			}

    			// no absorption
    			float Teff = 1 - Reff;
                write_to_vec(theta_1, wl, Reff, Teff);
    		}

    	}

    }

    float InterferenceTable::calc_recur_R(const float n1, const float n2, const float n3, const float theta_1, const float theta_2, const float theta_3, const int wl, const int d)
    {
    	// bugs to be fixed:
    	// when caluclating theta_3 at crit_angle, value of angle_asin can be "nan"

    	float rs1 = calc_rs(n1, n2, theta_1, theta_2);
    	float rp1 = calc_rp(n1, n2, theta_1, theta_2);
    	float rs2 = calc_rs(n2, n3, theta_2, theta_3);
    	float rp2 = calc_rp(n2, n3, theta_2, theta_3);

    	if(_debug) ss_log << "rs1:" << rs1 << ", rp1:" << rp1 << ", rs2:" << rs2 << ", rp2:" << rp2 << "\n";

    	float ts1 = rs1+1;
    	float tp1 = (rp1+1)*n1/n2;
    	float ts2 = rs2+1;
    	float tp2 = (rp2+1)*n2/n1;

    	if(_debug) ss_log << "ts1:" << ts1 << ", tp1:" << tp1 << ", ts2:" << ts2 << ", tp2:" << tp2 << "\n";

    	float phi = 2*2*(PI/wl)*n2*d*angle_cos(theta_2);
    	std::complex<float> one(1,0);
    	std::complex<float> i(0,1);
    	std::complex<float> phase_diff = std::exp(-1*phi*i);
    	std::complex<float> phase_diff_half = std::exp(-1*(phi/2)*i);
    	std::complex<float> rs_recur = (rs1*one + rs2*phase_diff) / (one+rs1*rs2*phase_diff);
    	std::complex<float> rp_recur = (rp1*one + rp2*phase_diff) / (one+rp1*rp2*phase_diff);
    	std::complex<float> ts_recur = (ts1*ts2*phase_diff_half) / (one+rs1*rs2*phase_diff);
    	std::complex<float> tp_recur = (tp1*tp2*phase_diff_half) / (one+rp1*rp2*phase_diff);

    	if(_debug) ss_log << "rs:" << rs_recur << ", rp:" << rp_recur << ", ts:" << ts_recur << ", tp:" << tp_recur << "\n";

    	float abs_s = abs(rs_recur);
    	float abs_p = abs(rp_recur);
    	float Reff = (abs_s*abs_s+abs_p*abs_p)/2;

    	float abs_ts = abs(ts_recur);
    	float abs_tp = abs(tp_recur);
    	float Teff = (abs_ts*abs_ts+abs_tp*abs_tp)/2;


    	if(_debug) ss_log << "|rs|:" << abs_s << ", |rp|:" << abs_p << ", Reff:" << Reff << "\n";
    	if(_debug) ss_log << "|ts|:" << abs_ts << ", |tp|:" << abs_tp << ", Teff:" << Teff << "\n\n";

    	return Reff;
    }

    float InterferenceTable::calc_rs(const float n1, const float n2, const float theta_i, const float theta_t)
    {
    	if (theta_i == 90) return -1;
    	if (theta_t == 90) return 1;
    	float p1 = n1*angle_cos(theta_i);
    	float p2 = n2*angle_cos(theta_t);
    	return (p1-p2)/(p1+p2);
    }

    float InterferenceTable::calc_rp(const float n1, const float n2, const float theta_i, const float theta_t)
    {
    	if (theta_i == 90) return -1;
    	if (theta_t == 90) return 1;
    	float p1 = n2*angle_cos(theta_i);
    	float p2 = n1*angle_cos(theta_t);
    	return (p1-p2)/(p1+p2);
    }


}// end namespace ph
