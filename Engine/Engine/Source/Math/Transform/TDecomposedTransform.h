#pragma once

#include "Math/TVector3.h"
#include "Math/TQuaternion.h"
#include "Math/TMatrix4.h"
#include "Math/math.h"

#include <Common/assertion.h>

#include <cstdlib>

namespace ph::math
{

/*! @brief Perform affine transformations in decomposed form.

Represents affine transformations in decomposed form. Specifically,
decomposing an transformation into translation, rotation, and scale;
each of the transformation component can be accessed and modified
directly.
*/
template<typename T>
class TDecomposedTransform final
{
public:
	/*! @brief Creates a transformation that will not have any effect.
	Defaults to identity transform.
	*/
	TDecomposedTransform();

	/*! @brief Creates a transformation.
	The transform effectively applies `scale` and `rotation` first, then translates to `position`.
	*/
	TDecomposedTransform(
		const TVector3<T>& position,
		const TQuaternion<T>& rotation,
		const TVector3<T>& scale);

	inline TDecomposedTransform& translate(const TVector3<T>& amount)
	{
		return translate(amount.x(), amount.y(), amount.z());
	}

	inline TDecomposedTransform& translate(const T x, const T y, const T z)
	{
		m_position.addLocal({x, y, z});

		return *this;
	}

	inline TDecomposedTransform rotate(const TQuaternion<T>& rotation)
	{
		TQuaternion<T> addtionalRotation(rotation);
		addtionalRotation.normalizeLocal();

		const TQuaternion<T> totalRotation = addtionalRotation.mul(m_rotation).normalizeLocal();
		setRotation(totalRotation);

		return *this;
	}

	inline TDecomposedTransform& rotate(const TVector3<T>& axis, const T degrees)
	{
		const TVector3<T> normalizedAxis(axis.normalize());
		rotate(TQuaternion<T>(normalizedAxis, math::to_radians(degrees)));

		return *this;
	}

	inline TDecomposedTransform& scale(const TVector3<T>& amount)
	{
		return scale(amount.x(), amount.y(), amount.z());
	}

	inline TDecomposedTransform& scale(const T x, const T y, const T z)
	{
		m_scale.mulLocal({x, y, z});

		return *this;
	}

	inline TDecomposedTransform& setPosition(const TVector3<T>& position)
	{
		m_position = position;

		return *this;
	}

	inline TDecomposedTransform& setRotation(const TQuaternion<T>& rotation)
	{
		m_rotation = rotation;

		return *this;
	}

	inline TDecomposedTransform& setScale(const T scale)
	{
		return setScale(TVector3<T>(scale, scale, scale));
	}

	inline TDecomposedTransform& setScale(const TVector3<T>& scale)
	{
		m_scale = scale;

		return *this;
	}

	TVector3<T> getPosition() const;
	TQuaternion<T> getRotation() const;
	TVector3<T> getScale() const;

	void genTransformMatrix(TMatrix4<T>* out_result) const;
	void genInverseTransformMatrix(TMatrix4<T>* out_result) const;

	// Inverts the transformation components. The effect of inverted and 
	// un-inverted transforms will cancel each other out.
	TDecomposedTransform invert() const;

	bool hasScaleEffect(T margin = 0) const;
	bool isScaleUniform(T margin = 0) const;
	bool isIdentity() const;

	bool operator == (const TDecomposedTransform& rhs) const;
	bool operator != (const TDecomposedTransform& rhs) const;

private:
	TVector3<T> m_position;
	TQuaternion<T> m_rotation;
	TVector3<T> m_scale;
};

// In-header Implementations:

template<typename T>
inline TDecomposedTransform<T>::TDecomposedTransform()
	: TDecomposedTransform(
		{0, 0, 0},
		TQuaternion<T>::makeNoRotation(),
		{1, 1, 1})
{}

template<typename T>
inline TDecomposedTransform<T>::TDecomposedTransform(
	const TVector3<T>& position,
	const TQuaternion<T>& rotation,
	const TVector3<T>& scale)

	: m_position(position)
	, m_rotation(rotation)
	, m_scale(scale)
{}

template<typename T>
inline void TDecomposedTransform<T>::genTransformMatrix(TMatrix4<T>* const out_result) const
{
	PH_ASSERT(out_result);

	TMatrix4<T> translationMatrix;
	TMatrix4<T> rotationMatrix;
	TMatrix4<T> scaleMatrix;
	translationMatrix.initTranslation(m_position);
	rotationMatrix.initRotation(m_rotation);
	scaleMatrix.initScale(m_scale);

	*out_result = translationMatrix.mul(rotationMatrix).mul(scaleMatrix);
}

template<typename T>
inline void TDecomposedTransform<T>::genInverseTransformMatrix(TMatrix4<T>* const out_result) const
{
	PH_ASSERT(out_result);

	TDecomposedTransform inverted = this->invert();

	TMatrix4<T> inverseTranslationMatrix;
	TMatrix4<T> inverseRotationMatrix;
	TMatrix4<T> inverseScaleMatrix;
	inverseTranslationMatrix.initTranslation(inverted.m_position);
	inverseRotationMatrix.initRotation(inverted.m_rotation);
	inverseScaleMatrix.initScale(inverted.m_scale);

	*out_result = inverseScaleMatrix.mul(inverseRotationMatrix).mul(inverseTranslationMatrix);
}

template<typename T>
inline TDecomposedTransform<T> TDecomposedTransform<T>::invert() const
{
	TDecomposedTransform result;
	result.m_position = m_position.mul(-1);
	result.m_rotation = m_rotation.conjugate();
	result.m_scale    = m_scale.rcp();
	return result;
}

template<typename T>
inline bool TDecomposedTransform<T>::hasScaleEffect(const T margin) const
{
	return std::abs(m_scale.x() - 1) > margin ||
	       std::abs(m_scale.y() - 1) > margin ||
	       std::abs(m_scale.z() - 1) > margin;
}

template<typename T>
inline bool TDecomposedTransform<T>::isScaleUniform(const T margin) const
{
	const T dSxSy = std::abs(m_scale.x() - m_scale.y());
	const T dSySz = std::abs(m_scale.y() - m_scale.z());
	const T dSzSx = std::abs(m_scale.z() - m_scale.x());

	return dSxSy < margin && dSySz < margin && dSzSx < margin;
}

template<typename T>
inline bool TDecomposedTransform<T>::isIdentity() const
{
	return *this == TDecomposedTransform();
}

template<typename T>
inline bool TDecomposedTransform<T>::operator == (const TDecomposedTransform& rhs) const
{
	return m_position == rhs.m_position &&
	       m_rotation == rhs.m_rotation &&
	       m_scale == rhs.m_scale;
}

template<typename T>
inline bool TDecomposedTransform<T>::operator != (const TDecomposedTransform& rhs) const
{
	return !(*this == rhs);
}

template<typename T>
inline TVector3<T> TDecomposedTransform<T>::getPosition() const
{
	return m_position;
}

template<typename T>
inline TQuaternion<T> TDecomposedTransform<T>::getRotation() const
{
	return m_rotation;
}

template<typename T>
inline TVector3<T> TDecomposedTransform<T>::getScale() const
{
	return m_scale;
}

}// end namespace ph::math
