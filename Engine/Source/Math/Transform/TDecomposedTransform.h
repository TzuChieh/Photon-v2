#pragma once

#include "Math/TVector3.tpp"
#include "Math/TQuaternion.tpp"
#include "Math/TMatrix4.tpp"
#include "Math/Math.h"

namespace ph
{

template<typename T>
class TDecomposedTransform final
{
public:
	TDecomposedTransform();

	inline void translate(const TVector3<T>& amount)
	{
		translate(amount.x, amount.y, amount.z);
	}

	inline void translate(const T x, const T y, const T z)
	{
		m_position.addLocal(x, y, z);
	}

	inline void rotate(const TQuaternion<T>& rotation)
	{
		TQuaternion<T> addtionalRotation(rotation);
		addtionalRotation.normalizeLocal();

		const TQuaternion<T> totalRotation = addtionalRotation.mul(m_rotation).normalizeLocal();
		setRotation(totalRotation);
	}

	inline void rotate(const TVector3<T>& axis, const T degrees)
	{
		const TVector3<T> normalizedAxis(axis.normalize());
		rotate(TQuaternion<T>(normalizedAxis, Math::toRadians(degrees)));
	}

	inline void scale(const TVector3<T>& amount)
	{
		scale(amount.x, amount.y, amount.z);
	}

	inline void scale(const T x, const T y, const T z)
	{
		m_scale.mulLocal(x, y, z);
	}

	inline void setPosition(const TVector3<T>& position)
	{
		m_position = position;
	}

	inline void setRotation(const TQuaternion<T>& rotation)
	{
		m_rotation = rotation;
	}

	inline void setScale(const TVector3<T>& scale)
	{
		m_scale = scale;
	}

	inline void get(TVector3<T>* const out_position, TQuaternion<T>* const out_rotation, TVector3<T>* const out_scale) const
	{
		*out_position = m_position;
		*out_rotation = m_rotation;
		*out_scale    = m_scale;
	}

	inline void genTransformMatrix(TMatrix4<T>* const out_result) const
	{
		TMatrix4<T> translationMatrix;
		TMatrix4<T> rotationMatrix;
		TMatrix4<T> scaleMatrix;
		translationMatrix.initTranslation(m_position);
		rotationMatrix.initRotation(m_rotation);
		scaleMatrix.initScale(m_scale);

		*out_result = translationMatrix.mul(rotationMatrix).mul(scaleMatrix);
	}

	inline void genInverseTransformMatrix(TMatrix4<T>* const out_result) const
	{
		TDecomposedTransform inverted = this->invert();

		TMatrix4<T> inverseTranslationMatrix;
		TMatrix4<T> inverseRotationMatrix;
		TMatrix4<T> inverseScaleMatrix;
		inverseTranslationMatrix.initTranslation(inverted.m_position);
		inverseRotationMatrix.initRotation(inverted.m_rotation);
		inverseScaleMatrix.initScale(inverted.m_scale);

		*out_result = inverseScaleMatrix.mul(inverseRotationMatrix).mul(inverseTranslationMatrix);
	}

	inline TDecomposedTransform invert() const;

private:
	TVector3<T>    m_position;
	TQuaternion<T> m_rotation;
	TVector3<T>    m_scale;
};

// Implementations:

template<typename T>
TDecomposedTransform<T>::TDecomposedTransform() : 
	m_position(0, 0, 0), m_rotation(0, 0, 0, 1), m_scale(1, 1, 1)
{

}

template<typename T>
inline TDecomposedTransform<T> TDecomposedTransform<T>::invert() const
{
	TDecomposedTransform result;
	result.m_position = m_position.mul(-1);
	result.m_rotation = m_rotation.conjugate();
	result.m_scale    = m_scale.reciprocal();
	return result;
}

}// end namespace ph