#pragma once

namespace ph { class Transform; }

namespace ph
{

class CookedMotion final
{
public:
	const Transform* localToWorld = nullptr;
	const Transform* worldToLocal = nullptr;
};

}// end namespace ph
