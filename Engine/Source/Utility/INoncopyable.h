#pragma once

namespace ph
{

class INoncopyable
{
protected:
	INoncopyable() = default;
	~INoncopyable() = default;

	INoncopyable(const INoncopyable& other) = delete;
	INoncopyable& operator = (const INoncopyable& rhs) = delete;
};

}// end namespace ph
