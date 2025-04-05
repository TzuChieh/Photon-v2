#pragma once

namespace ph
{

class Primitive;

class CookedNamedResource final
{
public:
	CookedNamedResource();

	const Primitive* getBackgroundPrimitive() const;
	void setBackgroundPrimitive(const Primitive* primitive);

private:
	const Primitive* m_backgroundPrimitive;
};

}// end namespace ph
