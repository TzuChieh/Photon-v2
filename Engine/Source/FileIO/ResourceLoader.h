#pragma once

#include <vector>
#include <string>
#include <memory>

namespace ph
{

class InputPacket;
class Camera;
class Film;
class Integrator;
class SampleGenerator;
class Geometry;
class Texture;
class Material;
class AModel;
class ALight;
class LightSource;

class ResourceLoader final
{
public:
	static std::unique_ptr<Camera> loadCamera(const InputPacket& packet);
	static std::unique_ptr<Film> loadFilm(const InputPacket& packet);
	static std::unique_ptr<Integrator> loadIntegrator(const InputPacket& packet);
	static std::unique_ptr<SampleGenerator> loadSampleGenerator(const InputPacket& packet);

	static std::shared_ptr<Geometry> loadGeometry(const InputPacket& packet);
	static std::shared_ptr<Texture> loadTexture(const InputPacket& packet);
	static std::shared_ptr<Material> loadMaterial(const InputPacket& packet);
	static std::shared_ptr<LightSource> loadLightSource(const InputPacket& packet);

	static std::unique_ptr<AModel> loadActorModel(const InputPacket& packet);
	static std::unique_ptr<ALight> loadActorLight(const InputPacket& packet);
};

}// end namespace ph