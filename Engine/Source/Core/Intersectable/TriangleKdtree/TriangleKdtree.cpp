#include "Core/Intersectable/TriangleKdtree/TriangleKdtree.h"
#include "Core/HitDetail.h"

#include <limits>

namespace ph
{

typedef struct s{
	KDNode *node;
    float tMin, tMax;
}KDQueue;

std::tuple<float,float,float,float,float,float> TriangleBound(Triangle *t, int index){
	float min_x = std::numeric_limits<float>::max();
	float max_x = std::numeric_limits<float>::lowest();

	float min_y = std::numeric_limits<float>::max();
	float max_y = std::numeric_limits<float>::lowest();

	float min_z = std::numeric_limits<float>::max();
	float max_z = std::numeric_limits<float>::lowest();

	for(int j = 0; j < 3; j++){
		if(t->getTverticies()[j].x < min_x){
			min_x = t->getTverticies()[j].x;
		}
		if(t->getTverticies()[j].y < min_y){
			min_y = t->getTverticies()[j].y;
		}
		if(t->getTverticies()[j].z < min_z){
			min_z = t->getTverticies()[j].z;
		}
		if(t->getTverticies()[j].x > max_x){
			max_x = t->getTverticies()[j].x;
		}
		if(t->getTverticies()[j].y > max_y){
			max_y = t->getTverticies()[j].y;
		}
		if(t->getTverticies()[j].z > max_z){
			max_z = t->getTverticies()[j].z;
		}
	}
	t->TBoundingBox.setVertexMin(min_x, min_y, min_z);

	t->TBoundingBox.setVertexMax(max_x, max_y, max_z);

	t->setIndex(index);
	
	return std::make_tuple(min_x,min_y,min_z,max_x,max_y,max_z);
}

void drawBounds(Voxel& V, Triangles& T){
	float min_x = std::numeric_limits<float>::max();
	float max_x = std::numeric_limits<float>::lowest();

	float min_y = std::numeric_limits<float>::max();
	float max_y = std::numeric_limits<float>::lowest();

	float min_z = std::numeric_limits<float>::max();
	float max_z = std::numeric_limits<float>::lowest();

	for(int i = 0; i < T.tris.size(); i++){
		auto local = TriangleBound(T.tris[i] , i);
		float local_min_x = std::get<0>(local);
		float local_min_y = std::get<1>(local);
		float local_min_z = std::get<2>(local);;
		float local_max_x = std::get<3>(local);;
		float local_max_y = std::get<4>(local);;
		float local_max_z = std::get<5>(local);;

		if(local_min_x < min_x){
			min_x = local_min_x;
		}
		if(local_min_y< min_y){
			min_y = local_min_y;
		}
		if(local_min_z < min_z){
			min_z = local_min_z;
		}
		if(local_max_x > max_x){
			max_x = local_max_x;
		}
		if(local_max_y > max_y){
			max_y = local_max_y;
		}
		if(local_max_z > max_z){
			max_z = local_max_z;
		}

	}
	//printf("minx:%f,min_y:%f,min_z:%f\n",min_x,min_y,min_z);
	V.box.setVertexMin(min_x, min_y, min_z);
	V.box.setVertexMax(max_x, max_y, max_z);
}

bool PointInAABB(Vec3 Point, AABB& Box){
    if(Point.x > Box.getVertexMin().x && Point.x < Box.getVertexMax().x &&
       Point.y > Box.getVertexMin().y && Point.y < Box.getVertexMax().y &&
       Point.z > Box.getVertexMin().z && Point.z < Box.getVertexMax().z)
        return true;
 	else
		return false;
}

bool TriangleInAABB(Triangle* tri, AABB& Box){
	if( PointInAABB(tri->getTverticies()[0], Box) &&
		PointInAABB(tri->getTverticies()[1], Box) &&
		PointInAABB(tri->getTverticies()[2], Box))
		return true;
	else
		return false;
}

//write R value of Triangles constructor, use std::move
Triangles Union(Triangles& T, Voxel& V){
	Triangles union_set = Triangles();
	for(int i = 0; i < T.tris.size(); i++){
		if( TriangleInAABB(T.tris[i], V.box) ){
			union_set.tris.push_back(T.tris[i]);
		}
	}
	return union_set;
}

//K_t: cost constant of traversal ,K_i: cost constant of intersection
float Cost(float K_t, float K_i, float P_left, float P_right, int left_traingles_n, int right_traingles_n){

	return K_t + K_i * (P_left * left_traingles_n + P_right * right_traingles_n);
}

float SA(Voxel& V){
	float dx = V.box.getVertexMax().x - V.box.getVertexMin().x;
	float dy = V.box.getVertexMax().y - V.box.getVertexMin().y;
	float dz = V.box.getVertexMax().z - V.box.getVertexMin().z;
	assert(dx>=0);
	assert(dy>=0);
	assert(dz>=0);
	return 2.0*( dx*dy + dx*dz + dy*dz );
}

void split_voxel(Voxel& V,Plane& P, Voxel& left_voxel, Voxel& right_voxel){

	left_voxel.box.setVertexMin(V.box.getVertexMin().x,V.box.getVertexMin().y,V.box.getVertexMin().z);
	right_voxel.box.setVertexMax(V.box.getVertexMax().x,V.box.getVertexMax().y,V.box.getVertexMax().z);

	switch(P.getNormal()){
		case math::X_AXIS:
			left_voxel.box.setVertexMax(-P.get_d(), V.box.getVertexMax().y, V.box.getVertexMax().z );
			right_voxel.box.setVertexMin(-P.get_d(), V.box.getVertexMin().y, V.box.getVertexMin().z );
			break;
		case math::Y_AXIS:
			left_voxel.box.setVertexMax(V.box.getVertexMax().x, -P.get_d(), V.box.getVertexMax().z );
			right_voxel.box.setVertexMin(V.box.getVertexMin().x, -P.get_d(), V.box.getVertexMin().z );
			break;
		case math::Z_AXIS:
			left_voxel.box.setVertexMax(V.box.getVertexMax().x, V.box.getVertexMax().y , -P.get_d() );
			right_voxel.box.setVertexMin(V.box.getVertexMin().x, V.box.getVertexMin().y, -P.get_d());
			break;
	}
}

float SAH(Plane& p, Voxel& V, int left_traingles_n, int right_traingles_n){
	//V split by p, use parameters to update left_voxel and right_voxel.
	Voxel left_voxel;
	Voxel right_voxel;
	split_voxel(V, p, left_voxel, right_voxel);

	float total_SA = SA(V);
	float P_left = SA(left_voxel)/total_SA;
	float P_right = SA(right_voxel)/total_SA;
	//choose the samller cost due to overlapping will cause different cost 
	float K_t = traversal_constant;
	float K_i = intersection_constant;
	/*
	//cancel parameter overlap_traingles_n 
	float cost_left = Cost(K_t, K_i, P_left, P_right, left_traingles_n + overlap_triangles_n, right_traingles_n);
	float cost_right = Cost(K_t, K_i, P_left, P_right, left_traingles_n, right_traingles_n + overlap_triangles_n);
	if(cost_left < cost_right){
		return std::make_tuple(cost_left, LEFT);
	}
	else 
		return std::make_tuple(cost_right, RIGHT);
	*/
	return Cost(K_t, K_i, P_left, P_right, left_traingles_n, right_traingles_n);
}




void setTriBoundingEdge(Triangles& T, int LongestAxis, std::vector<BoundEdge>& Edges){
	for(int i = 0; i < T.tris.size(); i++){
		auto LeftAndRightBound = T.tris[i]->getBoundingEdge(LongestAxis);
		/*
		BoundEdge left;
		left.axis = get<0>(LeftAndRightBound);
		left.EdgeType = BEGIN_EDGE;
		Edge.push_back(left);
		*/
		Edges.push_back( { std::get<0>(LeftAndRightBound) , BEGIN_EDGE} );


		//Edge.push_back( get<1>(LeftAndRightBound) );
		Edges.push_back( { std::get<1>(LeftAndRightBound) , END_EDGE} );

	} 
}

bool terminate(Triangles& T, Voxel& V, int depth){
	if(depth > 100 || T.tris.size() < 2){
		return 1;
	}
	else
		return 0;
}




//becareful of overlapping
bool edgeCmp(BoundEdge a, BoundEdge b){

	if(a.getSplitPos() == b.getSplitPos()){
		/*
		if(a.getEdgeType() == 0 && b.getEdgeType() == 1){
			return 0;
		}
		else if(b.getEdgeType() == 1 && a.getEdgeType() == 0){
			return 1;
		}
		*/
		return ( a.getEdgeType() > b.getEdgeType() );
	}

	return ( a.getSplitPos() < b.getSplitPos() );
}

Plane find_plane(Triangles& T, Voxel& V){

	int LongestAxis = V.LongestAxis();
	std::vector<BoundEdge> Edges;
	//use parameters to update BoundEdge
	setTriBoundingEdge(T, LongestAxis, Edges);
	//sort edges
	std::sort(Edges.begin(), Edges.end(), edgeCmp);
	int num_left_tris = 0;
	int num_on_plane_tris = 0;
	int num_right_tris = T.tris.size();
	
	float COST_MIN = FLT_MAX;
	Plane best_plane;
	//iterate all edges candidate
	for(int i = 0; i < Edges.size(); i++){
		if( Edges[i].getEdgeType() == 1){
			num_right_tris --;
		}
		Plane p(Edges[i], LongestAxis);
		//SAH(Plane& p, Voxel& V, int left_traingles_n, int right_traingles_n, int overlap_triangles_n) overlap is not used

		float cost = SAH(p,V,num_left_tris,num_right_tris);
		if(cost<COST_MIN){
			best_plane = p;
		    COST_MIN = cost;
		}
		if( Edges[i].getEdgeType() == 0){
			num_left_tris ++;
		}
	}
	return best_plane;
}

KDNode* KDNode::recBuild(Triangles& T, Voxel& V, int depth){
	if(terminate(T,V,depth)){
		KDNode *root = new KDNode(m_metadata);
		root->left = NULL;
		root->right = NULL;
		root->Tprim = T;
		return root;
		//return leaf_node(T)
	}

	Plane p = find_plane(T,V);
	
	Voxel left_voxel;
	Voxel right_voxel;

	//V split by p, use parameters to update left_voxel and right_voxel.
	split_voxel(V, p, left_voxel, right_voxel);

	Triangles left_tris;
	Triangles right_tris;

	//left_tris beyond in left_voxel , so does right_tris.
	left_tris = Union(T , left_voxel);
	right_tris = Union(T , right_voxel);

	KDNode *root = new KDNode(m_metadata);
	root->Tprim = T;
	root->left = recBuild(left_tris, left_voxel, depth+1);
	root->right = recBuild(right_tris, right_voxel, depth+1);
	root->plane = p;
	return root;
}

KDNode *KDtree_root;
Voxel World_Voxel;
KDNode* KDNode::build_KD_tree(Triangles& T){
	//drawBounds can only call once
	drawBounds(World_Voxel,T);
	KDtree_root = recBuild(T,World_Voxel,0);
	return KDtree_root;
}
//implement virtual functions of primitive.h
//1. implement virtual bool isIntersecting(const Ray& ray, HitProbe& probe) const = 0;
bool KDNode::isIntersecting(const Ray& ray, HitProbe& probe) const {
	//return false;
	float tMin , tMax;
	bool is_hit = 0;
	KDNode *cur_node = &KDtree_root[0];
	if( !World_Voxel.intersect( ray, World_Voxel, &tMin, &tMax) ){
		return false;
	}
	
	KDQueue queue[64];
	int todoPos = 63;
	float invDir[3];
	float rayDir[3];
	rayDir[0] = ray.getDirection().x;
	rayDir[1] = ray.getDirection().y;
	rayDir[2] = ray.getDirection().z;
	invDir[0] = 1/ray.getDirection().x;
	invDir[1] = 1/ray.getDirection().y;
	invDir[2] = 1/ray.getDirection().z;
	

	while(cur_node != NULL){
		if(ray.getMaxT() < tMin)
			break;
		

		if(!cur_node->isLeaf()){
			int split_axis = cur_node->plane.getNormal();
			float split_pos = cur_node->plane.get_d();
			float tPlane = (split_pos - ray.getOrigin()[split_axis]) * invDir[split_axis];
			bool left_first = (ray.getOrigin()[split_axis] < split_pos) 
								|| (ray.getOrigin()[split_axis] == split_pos && rayDir[split_axis]<=0);
			KDNode *cand1,*cand2;
			if(left_first){
				cand1 = cur_node->left;
				cand2 = cur_node->right;
			}
			else{
				cand1 = cur_node->right;
				cand2 = cur_node->left;
			}
			if (tPlane > tMax || tPlane <= 0)
                cur_node = cand1;
            else if (tPlane < tMin)
                cur_node = cand2;
            else { 
                queue[todoPos].node = cand2;
                queue[todoPos].tMin = tPlane;
                queue[todoPos].tMax = tMax;
                ++todoPos;

                cur_node = cand1;
                tMax = tPlane;
            }
		}
		else{
			for(int i = 0; i < cur_node->Tprim.tris.size(); i++){
				Triangle* triangle = cur_node->Tprim.tris[i];
				float hitT;
				if(triangle->Intersect(ray, &hitT)){
					is_hit = true;
					probe.pushBaseHit(this, hitT);
					probe.cachePointer(triangle);
					break;
				}
			}
			if(todoPos > 0 ){
				--todoPos;
				cur_node = queue[todoPos].node;
				tMin = queue[todoPos].tMin;
				tMax = queue[todoPos].tMax;
			}
		}

		
	}
	return is_hit;
}
//2. implement virtual void calcIntersectionDetail(const Ray& ray, HitProbe& probe, HitDetail* out_detail) const = 0
void KDNode::calcIntersectionDetail(const Ray& ray, HitProbe& probe, HitDetail* out_detail) const {

	Triangle* hitTriangle = (Triangle*)(probe.getCachedPointer());
	const Vector3R hitPosition = ray.getOrigin() + ray.getDirection() * probe.getHitRayT();


	out_detail->setMisc(this, Vector3R(0), probe.getHitRayT());
	out_detail->getHitInfo(ECoordSys::LOCAL).setAttributes(hitPosition, Vector3R(0, 1, 0), Vector3R(0, 1, 0));
	out_detail->getHitInfo(ECoordSys::WORLD) = out_detail->getHitInfo(ECoordSys::LOCAL);
}

//3. accept false positive, tell if two box overlapping.implement bool isIntersectingVolumeConservative(const AABB3D& volume) const = 0;
bool KDNode::isIntersectingVolumeConservative(const AABB3D& volume) const {
	return true;
}
	
//4. pointer send bounding box. implement void calcAABB(AABB3D* out_aabb) const = 0;
void KDNode::calcAABB(AABB3D* out_aabb) const {
	//setMinVertex(const Vector3R& minVertex)
	Vec3 myMinVec = World_Voxel.box.getVertexMin();	
	Vec3 myMaxVec = World_Voxel.box.getVertexMax();
	out_aabb->setMinVertex(Vector3R(myMinVec.x, myMinVec.y, myMinVec.z ));
	out_aabb->setMaxVertex(Vector3R(myMaxVec.x, myMaxVec.y, myMaxVec.z ));
}

}// end namespace ph

