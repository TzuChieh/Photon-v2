#pragma once

#include "Actor/Geometry/Geometry.h"
#include "Math/TVector3.h"
#include "Common/assertion.h"
#include "DataIO/SDL/sdl_interface.h"

namespace ph
{

class GTriangle : public Geometry
{
public:
	void genPrimitive(
		const PrimitiveBuildingMaterial& data,
		std::vector<std::unique_ptr<Primitive>>& out_primitives) const override;

	std::shared_ptr<Geometry> genTransformed(
		const math::StaticAffineTransform& transform) const override;

	bool isDegenerate() const;

	const math::Vector3R& getVa() const;
	const math::Vector3R& getVb() const;
	const math::Vector3R& getVc() const;
	const math::Vector3R& getNa() const;
	const math::Vector3R& getNb() const;
	const math::Vector3R& getNc() const;
	const math::Vector3R& getUVWa() const;
	const math::Vector3R& getUVWb() const;
	const math::Vector3R& getUVWc() const;

	GTriangle& setNa(const math::Vector3R& nA);
	GTriangle& setNb(const math::Vector3R& nB);
	GTriangle& setNc(const math::Vector3R& nC);
	GTriangle& setUVWa(const math::Vector3R& uvwA);
	GTriangle& setUVWb(const math::Vector3R& uvwB);
	GTriangle& setUVWc(const math::Vector3R& uvwC);

	GTriangle& setVertices(
		const math::Vector3R& vA, 
		const math::Vector3R& vB, 
		const math::Vector3R& vC);

private:
	math::Vector3R m_vA;
	math::Vector3R m_vB;
	math::Vector3R m_vC;

	math::Vector3R m_uvwA;
	math::Vector3R m_uvwB;
	math::Vector3R m_uvwC;

	math::Vector3R m_nA;
	math::Vector3R m_nB;
	math::Vector3R m_nC;

public:
	PH_DEFINE_SDL_CLASS(TOwnerSdlClass<GTriangle>)
	{
		ClassType clazz("triangle");
		clazz.docName("Triangle Geometry");
		clazz.description(
			"A single triangle-shaped surface. Please note that using this type of triangle for "
			"triangle mesh may induce significant overhead (in terms of memory usage). This is "
			"meant for very simple shapes or debug usages.");
		clazz.baseOn<Geometry>();

		TSdlVector3<OwnerType> vA("v-A", &OwnerType::m_vA);
		vA.description("The first vertex coordinates of the triangle.");
		vA.required();
		clazz.addField(vA);

		TSdlVector3<OwnerType> vB("v-B", &OwnerType::m_vB);
		vB.description("The second vertex coordinates of the triangle, in CCW.");
		vB.required();
		clazz.addField(vB);

		TSdlVector3<OwnerType> vC("v-C", &OwnerType::m_vC);
		vC.description("The third vertex coordinates of the triangle, in CCW.");
		vC.required();
		clazz.addField(vC);

		TSdlVector3<OwnerType> uvwA("uvw-A", &OwnerType::m_uvwA);
		uvwA.description("Texture coordinates of the first vertex.");
		uvwA.defaultTo({0, 0, 0});
		uvwA.optional();
		clazz.addField(uvwA);

		TSdlVector3<OwnerType> uvwB("uvw-B", &OwnerType::m_uvwB);
		uvwB.description("Texture coordinates of the first vertex.");
		uvwB.defaultTo({1, 0, 0});
		uvwB.optional();
		clazz.addField(uvwB);

		TSdlVector3<OwnerType> uvwC("uvw-C", &OwnerType::m_uvwC);
		uvwC.description("Texture coordinates of the first vertex.");
		uvwC.defaultTo({0, 1, 0});
		uvwC.optional();
		clazz.addField(uvwC);

		TSdlVector3<OwnerType> nA("n-A", &OwnerType::m_nA);
		nA.description("Normal vector of the first vertex.");
		nA.defaultTo({0, 0, 0});
		nA.optional();
		clazz.addField(nA);

		TSdlVector3<OwnerType> nB("n-B", &OwnerType::m_nB);
		nB.description("Normal vector of the second vertex.");
		nB.defaultTo({0, 0, 0});
		nB.optional();
		clazz.addField(nB);

		TSdlVector3<OwnerType> nC("n-C", &OwnerType::m_nC);
		nC.description("Normal vector of the third vertex.");
		nC.defaultTo({0, 0, 0});
		nC.optional();
		clazz.addField(nC);

		return clazz;
	}
};

// In-header Implementations:

inline const math::Vector3R& GTriangle::getVa() const
{
	return m_vA;
}

inline const math::Vector3R& GTriangle::getVb() const
{
	return m_vB;
}

inline const math::Vector3R& GTriangle::getVc() const
{
	return m_vC;
}

inline const math::Vector3R& GTriangle::getNa() const
{
	return m_nA;
}

inline const math::Vector3R& GTriangle::getNb() const
{
	return m_nB;
}

inline const math::Vector3R& GTriangle::getNc() const
{
	return m_nC;
}

inline const math::Vector3R& GTriangle::getUVWa() const
{
	return m_uvwA;
}

inline const math::Vector3R& GTriangle::getUVWb() const
{
	return m_uvwB;
}

inline const math::Vector3R& GTriangle::getUVWc() const
{
	return m_uvwC;
}

inline GTriangle& GTriangle::setNa(const math::Vector3R& nA)
{
	m_nA = nA;

	return *this;
}

inline GTriangle& GTriangle::setNb(const math::Vector3R& nB)
{
	m_nB = nB;

	return *this;
}

inline GTriangle& GTriangle::setNc(const math::Vector3R& nC)
{
	m_nC = nC;

	return *this;
}

inline GTriangle& GTriangle::setUVWa(const math::Vector3R& uvwA)
{
	m_uvwA = uvwA;

	return *this;
}

inline GTriangle& GTriangle::setUVWb(const math::Vector3R& uvwB)
{
	m_uvwB = uvwB;

	return *this;
}

inline GTriangle& GTriangle::setUVWc(const math::Vector3R& uvwC)
{
	m_uvwC = uvwC;

	return *this;
}

inline GTriangle& GTriangle::setVertices(
	const math::Vector3R& vA, 
	const math::Vector3R& vB, 
	const math::Vector3R& vC)
{
	m_vA = vA;
	m_vB = vB;
	m_vC = vC;

	return *this;
}

}// end namespace ph
