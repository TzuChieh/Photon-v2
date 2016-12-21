#pragma once

#include <memory>
#include <vector>

namespace ph
{

class Emitter;

class LightStorage final
{
public:
	LightStorage();

	void update();
	void addEmitter(const std::shared_ptr<Emitter>& emitter);

private:
	std::vector<std::shared_ptr<Emitter>> m_emitters;
};

}// end namespace ph