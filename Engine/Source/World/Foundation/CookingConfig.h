#pragma once

namespace ph
{

class CookingConfig final
{
public:
	bool preferTriangulated = false;
	bool preferIndexedVertices = false;

	/*! Force triangulated result. Generally this is handled by the base `Geometry` class and do
	not need to be explicitly handled in the derived classes. It is an error if triangulation 
	is not possible.
	*/
	bool forceTriangulated = false;
};

}// end namespace ph
