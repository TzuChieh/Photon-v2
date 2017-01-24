#include "Math/Vector3f.h"
#include "Math/Quaternion.h"
#include "FileIO/Tokenizer.h"

#include <vector>
#include <iostream>

namespace ph
{

const Vector3f Vector3f::UNIT_X_AXIS(1, 0, 0);
const Vector3f Vector3f::UNIT_Y_AXIS(0, 1, 0);
const Vector3f Vector3f::UNIT_Z_AXIS(0, 0, 1);
const Vector3f Vector3f::UNIT_NEGATIVE_X_AXIS(-1, 0, 0);
const Vector3f Vector3f::UNIT_NEGATIVE_Y_AXIS(0, -1, 0);
const Vector3f Vector3f::UNIT_NEGATIVE_Z_AXIS(0, 0, -1);

Vector3f::Vector3f(const std::string& stringRepresentation) : 
	Vector3f()
{
	const Tokenizer tokenizer({' ', '\t', '\n', '\r'}, {});
	std::vector<std::string> tokens;
	tokenizer.tokenize(stringRepresentation, tokens);

	if(tokens.size() != 3)
	{
		std::cerr << "warning: at Vector3f::Vector3f(), bad string representation <" + stringRepresentation + ">" << std::endl;
		return;
	}

	x = static_cast<float32>(std::stold(tokens[0]));
	y = static_cast<float32>(std::stold(tokens[1]));
	z = static_cast<float32>(std::stold(tokens[2]));
}

Vector3f Vector3f::rotate(const Quaternion& rotation) const
{
	Quaternion& conjugatedRotation = rotation.conjugate();
	Quaternion& result = rotation.mul(*this).mulLocal(conjugatedRotation);

	return Vector3f(result.x, result.y, result.z);
}

void Vector3f::rotate(const Quaternion& rotation, Vector3f* out_result) const
{
	Quaternion& conjugatedRotation = rotation.conjugate();
	Quaternion& result = rotation.mul(*this).mulLocal(conjugatedRotation);

	out_result->x = result.x;
	out_result->y = result.y;
	out_result->z = result.z;
}

}// end namespace ph