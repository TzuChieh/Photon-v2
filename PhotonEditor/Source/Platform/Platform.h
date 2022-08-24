#pragma once

#include <Common/assertion.h>
#include <Common/primitive_type.h>

#include <memory>

namespace ph::editor
{

class Input;
class Display;

class Platform
{
public:
	Platform(
		std::unique_ptr<Input>   input,
		std::unique_ptr<Display> display);

	inline virtual ~Platform() = default;

	virtual void update(float64 deltaS) = 0;

	const Input& getInput() const;
	const Display& getDisplay() const;

private:
	std::unique_ptr<Input>   m_input;
	std::unique_ptr<Display> m_display;
};

inline const Input& Platform::getInput() const
{
	PH_ASSERT(m_input);
	return *m_input;
}

inline const Display& Platform::getDisplay() const
{
	PH_ASSERT(m_display);
	return *m_display;
}

}// end namespace ph::editor
