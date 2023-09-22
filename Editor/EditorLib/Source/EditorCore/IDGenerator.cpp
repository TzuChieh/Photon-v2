#include "EditorCore/IDGenerator.h"
#include "EditorCore/Program.h"

#include <Math/Random/Pcg64DXSM.h>
#include <Utility/string_utils.h>

#include <atomic>
#include <random>

namespace ph::editor
{

namespace
{

std::atomic<uint64> counter(0);

math::Pcg64DXSM make_rng()
{
	std::random_device rd;
	return math::Pcg64DXSM(rd(), rd(), rd(), rd());
}

}// end anonymous namespace

uint64 IDGenerator::nextCount()
{
	return counter.fetch_add(1, std::memory_order_relaxed);
}

uint64 IDGenerator::nextTimestampedCount()
{
	return Program::getStartTime() + nextCount();
}

uint64 IDGenerator::nextRandomNumber()
{
	static thread_local math::Pcg64DXSM rng = make_rng();
	return rng.generate();
}

std::string IDGenerator::toString(uint64 id, int base)
{
	std::string str;
	str.resize(64);
	auto strSize = string_utils::stringify_int(id, str.data(), str.size(), base);
	str.resize(strSize);
	return str;
}

std::string IDGenerator::toString(uint64 id1, uint64 id2, int base)
{
	std::string str;
	str.resize(64 * 2 + 1);

	// Write `id1`
	auto strSize = string_utils::stringify_int(id1, str.data(), str.size(), base);

	// Write a dash
	str[strSize] = '-';
	++strSize;

	// Write `id2`
	strSize += string_utils::stringify_int(id2, str.data() + strSize, str.size() - strSize, base);

	str.resize(strSize);
	return str;
}

}// end namespace ph::editor
