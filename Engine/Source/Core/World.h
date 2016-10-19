#pragma once

#include <vector>

namespace ph
{

class Primitive;

class World final
{
public:
	//void addPrimitive();

private:
	std::vector<Primitive> m_primitives;
};

}// end namespace ph