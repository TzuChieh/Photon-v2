#include "Math/Transform/DynamicLinearTranslation.h"
#include "Core/Quantity/Time.h"

namespace ph
{

DynamicLinearTranslation::DynamicLinearTranslation(const Vector3R& posT0, 
                                                   const Vector3R& posT1) : 
	m_posT0(posT0), m_posT1(posT1)
{
	//std::cout << "posT0: " << m_posT0.toStringFormal() << std::endl;
	//std::cout << "posT1: " << m_posT1.toStringFormal() << std::endl;
}

DynamicLinearTranslation::~DynamicLinearTranslation() = default;

std::unique_ptr<Transform> DynamicLinearTranslation::genInversed() const
{
	return std::make_unique<DynamicLinearTranslation>(m_posT0.mul(-1), 
	                                                  m_posT1.mul(-1));
}

void DynamicLinearTranslation::transformVector(const Vector3R& vector, 
                                               const Time&     time,
                                               Vector3R* const out_vector) const
{
	*out_vector = vector;
}

void DynamicLinearTranslation::transformOrientation(const Vector3R& orientation, 
                                                    const Time&     time,
                                                    Vector3R* const out_orientation) const
{
	*out_orientation = orientation;
}

void DynamicLinearTranslation::transformPoint(const Vector3R& point, 
                                              const Time&     time,
                                              Vector3R* const out_point) const
{
	// TODO: change pos to translation to avoid potential misunderstanding

	//std::cerr << "t: " << time.relativeT << std::endl;

	*out_point = point.add(Vector3R::lerp(m_posT0, m_posT1, time.relativeT));
}

void DynamicLinearTranslation::transformLineSegment(const Vector3R& lineStartPos, 
                                                    const Vector3R& lineDir,
                                                    const real      lineMinT, 
                                                    const real      lineMaxT,
                                                    const Time&     time,
                                                    Vector3R* const out_lineStartPos, 
                                                    Vector3R* const out_lineDir,
                                                    real* const     out_lineMinT, 
                                                    real* const     out_lineMaxT) const
{
	DynamicLinearTranslation::transformPoint(lineStartPos, time, out_lineStartPos);
	*out_lineDir  = lineDir;
	*out_lineMinT = lineMinT;
	*out_lineMaxT = lineMaxT;
}

}// end namespace ph