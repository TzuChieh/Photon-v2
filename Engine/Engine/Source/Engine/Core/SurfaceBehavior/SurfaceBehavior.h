#pragma once

#include <Common/assertion.h>
#include <Common/primitive_type.h>

namespace ph
{

class SurfaceOptics;
class SurfaceEmitter;

class SurfaceBehavior final
{
public:
	SurfaceBehavior();

	/*!
	Any non-null optics will be considered obstructive (and not obstructive otherwise).
	@param optics The optics to set. If null, the resulting surface will not have any optical effect
	and a no-op optics will be used instead.
	*/
	void setOptics(const SurfaceOptics* optics);

	/*!
	Any non-null emitter will be considered emissive (and not emissive otherwise).
	@param emitter The emitter to set. If null, the resulting surface will not have any emissive
	effect and a no-op emitter will be used instead.
	*/
	void setEmitter(const SurfaceEmitter* emitter);

	/*!
	The returned optics may have no effect during rendering. Use `isObstructive()` to check if
	the optics has any effect before a more involved computation starts.
	*/
	const SurfaceOptics& getOptics() const;

	/*
	The returned emitter may have no effect during rendering. Use `isEmissive()` to check if
	the emitter has any effect before a more involved computation starts.
	*/
	const SurfaceEmitter& getEmitter() const;

	bool isObstructive() const;
	bool isEmissive() const;

private:
	const SurfaceOptics* m_optics;
	const SurfaceEmitter* m_emitter;
	uint32f m_isObstructive : 1;
	uint32f m_isEmissive : 1;
};

inline const SurfaceOptics& SurfaceBehavior::getOptics() const
{
	PH_ASSERT(m_optics);
	return *m_optics;
}

inline const SurfaceEmitter& SurfaceBehavior::getEmitter() const
{
	PH_ASSERT(m_emitter);
	return *m_emitter;
}

inline bool SurfaceBehavior::isObstructive() const
{
	return m_isObstructive;
}

inline bool SurfaceBehavior::isEmissive() const
{
	return m_isEmissive;
}

}// end namespace ph
