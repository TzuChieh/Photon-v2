#pragma once

namespace ph
{

class InputPacket;

class FunctionExecutor final
{
public:
	static void executeTransform(const InputPacket& packet);
};

}// end namespace ph