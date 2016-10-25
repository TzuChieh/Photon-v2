#pragma once

namespace ph
{

class World;
class HDRFrame;
class Camera;

class PathTracer final
{
public:
	void trace(const Camera& camera, const World& world, HDRFrame* const out_hdrFrame) const;
};

}// end namespace ph