from mathutils import Vector, Quaternion


def to_photon_vec3(bvec3):
	return Vector((bvec3.y, bvec3.z, bvec3.x))


def to_photon_quat(bquat):
	return Quaternion((bquat.w, bquat.y, bquat.z, bquat.x))

