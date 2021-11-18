#include "Math/math.h"
#include "Math/TVector3.h"
#include "Math/constant.h"
#include "Common/assertion.h"

#include <iostream>
#include <iomanip>
#include <cmath>

namespace ph::math
{

void form_orthonormal_basis_frisvad(const Vector3R& unitYaxis, Vector3R* const out_unitXaxis, Vector3R* const out_unitZaxis)
{
	// Handle the singularity occurred when y is close to -1.
	if(unitYaxis.y() < -0.9999999_r)
	{
		out_unitXaxis->set({-1.0_r, 0.0_r, 0.0_r});
		out_unitZaxis->set({0.0_r, 0.0_r, -1.0_r});
		return;
	}
	
	const real commonReci = 1.0_r / (1.0_r + unitYaxis.y());
	const real commonTerm = -unitYaxis.x() * unitYaxis.z() * commonReci;
	out_unitXaxis->set({1.0_r - unitYaxis.x() * unitYaxis.x() * commonReci, -unitYaxis.x(), commonTerm});
	out_unitZaxis->set({commonTerm, -unitYaxis.z(), 1.0_r - unitYaxis.z() * unitYaxis.z() * commonReci});

	// TEST
	/*std::cerr << std::setprecision(20);
	if(std::abs(out_unitXaxis->length() - 1.0_r) > 0.001_r)
		std::cerr << out_unitXaxis->length() << std::endl;
	if(std::abs(out_unitZaxis->length() - 1.0_r) > 0.001_r)
		std::cerr << out_unitZaxis->length() << std::endl;
	if(out_unitXaxis->dot(*out_unitZaxis) > 0.001_r)
		std::cerr << out_unitXaxis->dot(*out_unitZaxis) << std::endl;
	if(out_unitZaxis->dot(unitYaxis) > 0.001_r)
		std::cerr << out_unitZaxis->dot(unitYaxis) << std::endl;
	if(unitYaxis.dot(*out_unitXaxis) > 0.001_r)
		std::cerr << unitYaxis.dot(*out_unitXaxis) << std::endl;*/
}

bool is_same_hemisphere(const Vector3R& vector, const Vector3R& N)
{
	PH_ASSERT(N.length() > 0.0_r);

	return N.dot(vector) >= 0.0_r;
}

}// end namespace ph::math
