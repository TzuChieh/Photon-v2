#pragma once

namespace ph
{

class World;
class HdrFrame;
class Camera;

class PathTracer final
{
public:
	void trace(const Camera& camera, const World& world, HdrFrame* const out_hdrFrame) const;
};

}// end namespace ph