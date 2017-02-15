bool PTriangle::isIntersecting(const Ray& ray, Intersection* const out_intersection) const
{
	Ray localRay;
	m_metadata->worldToLocal.transformRay(ray, &localRay);

	// hitT's unit is in world space (localRay's direction can have scale factor)
	const real hitT = localRay.getOrigin().sub(m_vA).dot(m_faceNormal) / (-localRay.getDirection().dot(m_faceNormal));

	// reject by distance (NaN-aware)
	if(!(ray.getMinT() < hitT && hitT < ray.getMaxT()))
		return false;

	// projected hit point
	real hitPu, hitPv;

	// projected side vector AB and AC
	real abPu, abPv, acPu, acPv;

	// find dominant axis
	if(abs(m_faceNormal.x) > abs(m_faceNormal.y))
	{
		// X dominant, projection plane is YZ
		if(abs(m_faceNormal.x) > abs(m_faceNormal.z))
		{
			hitPu = hitT * localRay.getDirection().y + localRay.getOrigin().y - m_vA.y;
			hitPv = hitT * localRay.getDirection().z + localRay.getOrigin().z - m_vA.z;
			abPu = m_eAB.y;
			abPv = m_eAB.z;
			acPu = m_eAC.y;
			acPv = m_eAC.z;
		}
		// Z dominant, projection plane is XY
		else
		{
			hitPu = hitT * localRay.getDirection().x + localRay.getOrigin().x - m_vA.x;
			hitPv = hitT * localRay.getDirection().y + localRay.getOrigin().y - m_vA.y;
			abPu = m_eAB.x;
			abPv = m_eAB.y;
			acPu = m_eAC.x;
			acPv = m_eAC.y;
		}
	}
	// Y dominant, projection plane is ZX
	else if(abs(m_faceNormal.y) > abs(m_faceNormal.z))
	{
		hitPu = hitT * localRay.getDirection().z + localRay.getOrigin().z - m_vA.z;
		hitPv = hitT * localRay.getDirection().x + localRay.getOrigin().x - m_vA.x;
		abPu = m_eAB.z;
		abPv = m_eAB.x;
		acPu = m_eAC.z;
		acPv = m_eAC.x;
	}
	// Z dominant, projection plane is XY
	else
	{
		hitPu = hitT * localRay.getDirection().x + localRay.getOrigin().x - m_vA.x;
		hitPv = hitT * localRay.getDirection().y + localRay.getOrigin().y - m_vA.y;
		abPu = m_eAB.x;
		abPv = m_eAB.y;
		acPu = m_eAC.x;
		acPv = m_eAC.y;
	}

	// TODO: check if these operations are possible of producing NaNs

	// barycentric coordinate of vertex B in the projected plane
	const real baryB = (hitPu*acPv - hitPv*acPu) / (abPu*acPv - abPv*acPu);
	if(baryB < 0.0_r) return false;

	// barycentric coordinate of vertex C in the projected plane
	const real baryC = (hitPu*abPv - hitPv*abPu) / (acPu*abPv - abPu*acPv);
	if(baryC < 0.0_r) return false;

	if(baryB + baryC > 1.0_r) return false;

	// so the ray intersects the triangle (TODO: reuse calculated results!)

	Vector3R hitPosition;
	Vector3R hitNormal;
	Vector3R localHitNormal(m_nA.mul(1.0_r - baryB - baryC).addLocal(m_nB.mul(baryB)).addLocal(m_nC.mul(baryC)));
	m_metadata->localToWorld.transformPoint(localRay.getDirection().mul(hitT).addLocal(localRay.getOrigin()), &hitPosition);
	m_metadata->localToWorld.transformVector(localHitNormal, &hitNormal);
	//m_parentModel->getModelToWorldTransform()->transformVector(m_faceNormal, &hitNormal);

	Vector3R hitGeoNormal;
	m_metadata->localToWorld.transformVector(m_faceNormal, &hitGeoNormal);

	out_intersection->setHitPosition(hitPosition);
	out_intersection->setHitSmoothNormal(hitNormal.normalizeLocal());
	out_intersection->setHitGeoNormal(hitGeoNormal.normalizeLocal());
	out_intersection->setHitUVW(m_uvwA.mul(1.0_r - baryB - baryC).addLocal(m_uvwB.mul(baryB)).addLocal(m_uvwC.mul(baryC)));
	out_intersection->setHitPrimitive(this);

	return true;
}
