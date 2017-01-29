#include "Math/TransformInfo.h"

namespace ph
{

TransformInfo::TransformInfo() :
	m_position(0, 0, 0), m_rotation(0, 0, 0, 1), m_scale(1, 1, 1)
{
	
}

Transform TransformInfo::genTransform(const Transform& parentTransform) const
{
	Matrix4f translationMatrix;
	Matrix4f rotationMatrix;
	Matrix4f scaleMatrix;
	translationMatrix.initTranslation(m_position);
	rotationMatrix.initRotation(m_rotation);
	scaleMatrix.initScale(m_scale);

	return parentTransform.transform(Transform(translationMatrix.mul(rotationMatrix).mul(scaleMatrix)));
}

Transform TransformInfo::genInverseTransform(const Transform& parentInvTransform) const
{
	Matrix4f invTranslationMatrix;
	Matrix4f invRotationMatrix;
	Matrix4f invScaleMatrix;
	invTranslationMatrix.initTranslation(m_position.mul(-1.0f));
	invRotationMatrix.initRotation(m_rotation.conjugate());
	invScaleMatrix.initScale(m_scale.reciprocal());

	return Transform(invScaleMatrix.mul(invRotationMatrix).mul(invTranslationMatrix)).transform(parentInvTransform);
}

}// end namespace ph