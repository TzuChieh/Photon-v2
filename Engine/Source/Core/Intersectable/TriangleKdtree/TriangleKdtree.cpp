#include "Core/Intersectable/TriangleKdtree/TriangleKdtree.h"
#include "Core/HitDetail.h"
#include <unistd.h>
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
	
	/*
	if(t->getVertexA().x < min_x){
		min_x = t->getVertexA().x;
	}
	if(t->getVertexA().y < min_y){
		min_y = t->getVertexA().y;
	}
	if(t->getVertexA().z < min_z){
		min_z = t->getVertexA().z;
	}
	if(t->getVertexA().x > max_x){
		max_x = t->getVertexA().x;
	}
	if(t->getVertexA().y > max_y){
		max_y = t->getVertexA().y;
	}
	if(t->getVertexA().z > max_z){
		max_z = t->getVertexA().z;
	}

	if(t->getVertexB().x < min_x){
		min_x = t->getVertexB().x;
	}
	if(t->getVertexB().y < min_y){
		min_y = t->getVertexB().y;
	}
	if(t->getVertexB().z < min_z){
		min_z = t->getVertexB().z;
	}
	if(t->getVertexB().x > max_x){
		max_x = t->getVertexB().x;
	}
	if(t->getVertexB().y > max_y){
		max_y = t->getVertexB().y;
	}
	if(t->getVertexB().z > max_z){
		max_z = t->getVertexB().z;
	}

	if(t->getVertexC().x < min_x){
		min_x = t->getVertexC().x;
	}
	if(t->getVertexC().y < min_y){
		min_y = t->getVertexC().y;
	}
	if(t->getVertexC().z < min_z){
		min_z = t->getVertexC().z;
	}
	if(t->getVertexC().x > max_x){
		max_x = t->getVertexC().x;
	}
	if(t->getVertexC().y > max_y){
		max_y = t->getVertexC().y;
	}
	if(t->getVertexC().z > max_z){
		max_z = t->getVertexC().z;
	}
	*/
	t->TBoundingBox.setMinVertex( Vector3R(min_x, min_y, min_z) );

	t->TBoundingBox.setMaxVertex( Vector3R(max_x, max_y, max_z) );

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
	V.box.setMinVertex(Vector3R(min_x, min_y, min_z));
	V.box.setMaxVertex(Vector3R(max_x, max_y, max_z));
}

bool PointInAABB3D(Vector3R Point, AABB3D& Box){
    if(Point.x > Box.getMinVertex().x && Point.x < Box.getMaxVertex().x &&
       Point.y > Box.getMinVertex().y && Point.y < Box.getMaxVertex().y &&
       Point.z > Box.getMinVertex().z && Point.z < Box.getMaxVertex().z)
        return true;
 	else
		return false;
}

bool TriangleOverlapAABB3D(Triangle* tri, AABB3D& Box){
	//base on http://fileadmin.cs.lth.se/cs/personal/tomas_akenine-moller/code/tribox_tam.pdf algorithm
	//return true if overlap
	//test 1

	float triVertexMax[3];
	float triVertexMin[3];
	//Compute AABB center
	Vector3R BoxCenter = (Box.getMinVertex() + Box.getMinVertex()) * 0.5f;
	//Compute Box translate to center at origin
	Vector3R Origin_box_Max = Box.getMaxVertex() - BoxCenter;
	Vector3R Origin_box_Min = Box.getMinVertex() - BoxCenter;
	triVertexMax[0] = tri->TBoundingBox.getMaxVertex().x - BoxCenter.x;
	triVertexMax[1] = tri->TBoundingBox.getMaxVertex().y - BoxCenter.y;
	triVertexMax[2] = tri->TBoundingBox.getMaxVertex().z - BoxCenter.z;
	
	triVertexMin[0] = tri->TBoundingBox.getMinVertex().x - BoxCenter.x;
	triVertexMin[1] = tri->TBoundingBox.getMinVertex().y - BoxCenter.y;
	triVertexMin[2] = tri->TBoundingBox.getMinVertex().z - BoxCenter.z;

	Vector3R edgeOA = tri->getTverticies()[1] - tri->getTverticies()[0];
	Vector3R edgeOB = tri->getTverticies()[2] - tri->getTverticies()[0];
	Vector3R edgeAB = tri->getTverticies()[1] - tri->getTverticies()[2];

	Vector3R a00(0, -edgeOA.z, edgeOA.y);
	Vector3R a01(0, -edgeOB.z, edgeOB.y);
	Vector3R a02(0, -edgeAB.z, edgeAB.y);

	Vector3R a10(edgeOA.z, 0, -edgeOA.x); 
	Vector3R a11(edgeOB.z, 0, -edgeOB.x); 
	Vector3R a12(edgeAB.z, 0, -edgeAB.x); 

	Vector3R a20(-edgeOA.y, edgeOA.x, 0);
	Vector3R a21(-edgeOB.y, edgeOB.x, 0);
	Vector3R a22(-edgeAB.y, edgeAB.x, 0);

	Vector3R v0 = tri->getTverticies()[0] - BoxCenter;
	Vector3R v1 = tri->getTverticies()[1] - BoxCenter;
	Vector3R v2 = tri->getTverticies()[2] - BoxCenter;
	float p0,p1,p2,r;
	//test 1
	//test all axises
	p0 = v0.dot( a00 );
	p1 = v1.dot( a00 );
	p2 = v2.dot( a00 );
	r = Origin_box_Max.y * std::abs( edgeOA.z ) + Origin_box_Max.z * std::abs( edgeOA.y );
	if ( fmax( -fMax3( p0, p1, p2 ), fMin3( p0, p1, p2 ) ) > r ) {
		// Axis is a separating axis
		return false; 
	}

	p0 = v0.dot( a01 );
	p1 = v1.dot( a01 );
	p2 = v2.dot( a01 );
	r = Origin_box_Max.y * std::abs( edgeOB.z ) + Origin_box_Max.z * std::abs( edgeOB.y );
	if ( fmax( -fMax3( p0, p1, p2 ), fMin3( p0, p1, p2 ) ) > r ) {
		// Axis is a separating axis
		return false; 
	}
	
	p0 = v0.dot( a02 );
	p1 = v1.dot( a02 );
	p2 = v2.dot( a02 );
	r = Origin_box_Max.y * std::abs( edgeAB.z ) + Origin_box_Max.z * std::abs( edgeAB.y );
	if ( fmax( -fMax3( p0, p1, p2 ), fMin3( p0, p1, p2 ) ) > r ) {
		// Axis is a separating axis
		return false; 
	}

	p0 = v0.dot( a10 );
	p1 = v1.dot( a10 );
	p2 = v2.dot( a10 );
	r = Origin_box_Max.x * std::abs( edgeOA.z ) + Origin_box_Max.z * std::abs( edgeOA.x );
	if ( fmax( -fMax3( p0, p1, p2 ), fMin3( p0, p1, p2 ) ) > r ) {
		// Axis is a separating axis
		return false; 
	}
	
	p0 = v0.dot( a11 );
	p1 = v1.dot( a11 );
	p2 = v2.dot( a11 );
	r = Origin_box_Max.x * std::abs( edgeOB.z ) + Origin_box_Max.z * std::abs( edgeOB.x );
	if ( fmax( -fMax3( p0, p1, p2 ), fMin3( p0, p1, p2 ) ) > r ) {
		// Axis is a separating axis
		return false; 
	}

	p0 = v0.dot( a12 );
	p1 = v1.dot( a12 );
	p2 = v2.dot( a12 );
	r = Origin_box_Max.x * std::abs( edgeAB.z ) + Origin_box_Max.z * std::abs( edgeAB.x );
	if ( fmax( -fMax3( p0, p1, p2 ), fMin3( p0, p1, p2 ) ) > r ) {
		// Axis is a separating axis
		return false; 
	}

	p0 = v0.dot( a20 );
	p1 = v1.dot( a20 );	
	p2 = v2.dot( a20 );
	r = Origin_box_Max.x * std::abs( edgeOA.y ) + Origin_box_Max.y * std::abs( edgeOA.x );
	if ( fmax( -fMax3( p0, p1, p2 ), fMin3( p0, p1, p2 ) ) > r ) {
		// Axis is a separating axis
		return false; 
	}

	p0 = v0.dot( a21 );
	p1 = v1.dot( a21 );	
	p2 = v2.dot( a21 );
	r = Origin_box_Max.x * std::abs( edgeOB.y ) + Origin_box_Max.y * std::abs( edgeOB.x );
	if ( fmax( -fMax3( p0, p1, p2 ), fMin3( p0, p1, p2 ) ) > r ) {
		// Axis is a separating axis
		return false; 
	}
	
	p0 = v0.dot( a22 );
	p1 = v1.dot( a22 );	
	p2 = v2.dot( a22 );
	r = Origin_box_Max.x * std::abs( edgeAB.y ) + Origin_box_Max.y * std::abs( edgeAB.x );
	if ( fmax( -fMax3( p0, p1, p2 ), fMin3( p0, p1, p2 ) ) > r ) {
		// Axis is a separating axis
		return false; 
	}

	//test 2
	//3 tests for 3 axis respectively see the AABB of Box and tri
	if ( fMax3( v0.x, v1.x, v2.x ) < -Origin_box_Max.x || fMin3( v0.x, v1.x, v2.x ) > Origin_box_Max.x ) {
		return false;
	}

	if ( fMax3( v0.y, v1.y, v2.y ) < -Origin_box_Max.y || fMin3( v0.y, v1.y, v2.y ) > Origin_box_Max.y ) {
		return false;
	}

	if ( fMax3( v0.z, v1.z, v2.z ) < -Origin_box_Max.z || fMin3( v0.z, v1.z, v2.z ) > Origin_box_Max.z ) {
		return false;
	}
	//test 3
	//test if a plane overlap a AABB, assume plane normal is triangle normal

	Vector3R triangle_normal = edgeOA.cross(edgeOB).normalize();
	//L(t) = box.center + t * triangle.normal
	r = Origin_box_Max.x * std::abs(triangle_normal.x) + Origin_box_Max.y * std::abs(triangle_normal.y) + Origin_box_Max.z * std::abs(triangle_normal.z);
	float plane_constant = triangle_normal.dot(tri->getTverticies()[0]);
	float dis_to_plane = triangle_normal.dot(BoxCenter) - plane_constant;

	return std::abs(dis_to_plane) <= r;
}

//write R value of Triangles constructor, use std::move
Triangles Union(Triangles& T, Voxel& V){
	Triangles union_set = Triangles();
	for(int i = 0; i < T.tris.size(); i++){
		if( TriangleOverlapAABB3D(T.tris[i], V.box) ){
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
	float dx = V.box.getMaxVertex().x - V.box.getMinVertex().x;
	float dy = V.box.getMaxVertex().y - V.box.getMinVertex().y;
	float dz = V.box.getMaxVertex().z - V.box.getMinVertex().z;
	PH_ASSERT_GE(dx, 0);
	PH_ASSERT_GE(dy, 0);
	PH_ASSERT_GE(dz, 0);
	return 2.0*( dx*dy + dx*dz + dy*dz );
}

void split_voxel(Voxel& V,Plane& P, Voxel& left_voxel, Voxel& right_voxel){

	left_voxel.box.setMinVertex(V.box.getMinVertex());
	right_voxel.box.setMaxVertex(V.box.getMaxVertex());

	switch(P.getNormal()){
		case math::X_AXIS:
			left_voxel.box.setMaxVertex(Vector3R(P.get_d(), V.box.getMaxVertex().y, V.box.getMaxVertex().z ));
			right_voxel.box.setMinVertex(Vector3R(P.get_d(), V.box.getMinVertex().y, V.box.getMinVertex().z ));
			break;
		case math::Y_AXIS:
			left_voxel.box.setMaxVertex( Vector3R(V.box.getMaxVertex().x, P.get_d(), V.box.getMaxVertex().z) );
			right_voxel.box.setMinVertex( Vector3R(V.box.getMinVertex().x, P.get_d(), V.box.getMinVertex().z) );
			break;
		case math::Z_AXIS:
			left_voxel.box.setMaxVertex( Vector3R(V.box.getMaxVertex().x, V.box.getMaxVertex().y , P.get_d()) );
			right_voxel.box.setMinVertex( Vector3R(V.box.getMinVertex().x, V.box.getMinVertex().y, P.get_d()) );
			break;
	}
}

float SAH(Plane& p, Voxel& V, int left_traingles_n, int right_traingles_n)
{
	//V split by p, use parameters to update left_voxel and right_voxel.
	Voxel left_voxel;
	Voxel right_voxel;
	split_voxel(V, p, left_voxel, right_voxel);
	printf("SAH !:left_voxel,Min%lf %lf %lf Max:%lf %lf %lf\n right_voxel,Min:%lf %lf %lf Max:%lf %lf %lf\n",left_voxel.box.getMinVertex().x,left_voxel.box.getMinVertex().y,left_voxel.box.getMinVertex().z,left_voxel.box.getMaxVertex().x,left_voxel.box.getMaxVertex().y,left_voxel.box.getMaxVertex().z,right_voxel.box.getMinVertex().x,right_voxel.box.getMinVertex().y,right_voxel.box.getMinVertex().z,right_voxel.box.getMaxVertex().x,right_voxel.box.getMaxVertex().y,right_voxel.box.getMaxVertex().z);
	float total_SA = SA(V);
	float P_left = SA(left_voxel)/total_SA;
	float P_right = SA(right_voxel)/total_SA;
	//choose the samller cost due to overlapping will cause different cost 
	float K_t = traversal_constant;
	float K_i = intersection_constant;
	return Cost(K_t, K_i, P_left, P_right, left_traingles_n, right_traingles_n);
}

void setTriBoundingEdge(Voxel& V, Triangles& T, int LongestAxis, std::vector<BoundEdge>& Edges)
{


	for(int i = 0; i < T.tris.size(); i++){
		auto LeftAndRightBound = T.tris[i]->getBoundingEdge(LongestAxis);
		/*
		BoundEdge left;
		left.axis = get<0>(LeftAndRightBound);
		left.EdgeType = BEGIN_EDGE;
		Edge.push_back(left);
		*/
		float left = std::get<0>(LeftAndRightBound);
		float right = std::get<1>(LeftAndRightBound); 
		
		Edges.push_back( { left , BEGIN_EDGE} );
		//Edge.push_back( get<1>(LeftAndRightBound) );
		Edges.push_back( { right , END_EDGE} );

	} 
}

bool terminate(Triangles& T, Voxel& V, int depth)
{
		//BUG,T.tris.size< 2 will have bug
	if(depth >= MAX_DEPTH || T.tris.size() < 10){
		return 1;
	}
	else
		return 0;
}




//becareful of overlapping
bool edgeCmp(BoundEdge a, BoundEdge b)
{
	if(a.getSplitPos() == b.getSplitPos()){
		/*
		if(a.getEdgeType() == 0 && b.getEdgeType() == 1){
			return 0;
		}
		else if(b.getEdgeType() == 1 && a.getEdgeType() == 0){
			return 1;
		}
		*/
		return ( a.getEdgeType() < b.getEdgeType() );
	}

	return ( a.getSplitPos() < b.getSplitPos() );
}

Plane find_plane(Triangles& T, Voxel& V)
{
	int LongestAxis = V.LongestAxis();
	printf("LongestAxis:%d\n",LongestAxis);
	std::vector<BoundEdge> Edges;
	//use parameters to update BoundEdge
	setTriBoundingEdge(V, T, LongestAxis, Edges);
	//sort edges
	std::sort(Edges.begin(), Edges.end(), edgeCmp);
	int num_left_tris = 0;
	int num_right_tris = T.tris.size();
	
	float Voxel_min;
	float Voxel_max;
	if(LongestAxis == math::X_AXIS)
	{
		Voxel_min = V.box.getMinVertex().x;
		Voxel_max = V.box.getMaxVertex().x;
	}
	else if(LongestAxis == math::Y_AXIS)
	{
		Voxel_min = V.box.getMinVertex().y;
		Voxel_max = V.box.getMaxVertex().y;
	}
	else if(LongestAxis == math::Z_AXIS)
	{
		Voxel_min = V.box.getMinVertex().z;
		Voxel_max = V.box.getMaxVertex().z;
	}
	PH_ASSERT_GE(Voxel_max, Voxel_min);

	float COST_MIN = std::numeric_limits<float>::max();
	Plane best_plane;
	//iterate all edges candidate
	printf("Edges.size:%lu\n",Edges.size());
	for(int i = 0; i < Edges.size(); i++){
		if( Edges[i].getEdgeType() == 1){
			num_right_tris --;
		}
		if( (Edges[i].getSplitPos() > Voxel_min) && (Edges[i].getSplitPos() < Voxel_max) )
		{
			Plane p(Edges[i], LongestAxis);
			//SAH(Plane& p, Voxel& V, int left_traingles_n, int right_traingles_n, int overlap_triangles_n) overlap is not used
			printf("d is :%lf\n",Edges[i].getSplitPos());
			float cost = SAH(p,V,num_left_tris,num_right_tris);
			if(cost<COST_MIN){
				best_plane = p;
				COST_MIN = cost;
			}
			if( Edges[i].getEdgeType() == 0){
				num_left_tris ++;
			}
			puts("inside!");
		}
	}

	return best_plane;
}
//std::shared_ptr<KDNode> KDNode::recBuild(Triangles& T, Voxel& V, int depth){
KDNode *KDNode::recBuild(Triangles& T, Voxel& V, int depth)
{
	//printf("depth:%d\n",depth);
	printf("recbuild T.size():%lu\n",T.tris.size());
	Plane p = find_plane(T,V);
	//when p.getNormal == -1, it means all the triangle bounding edge does not exist inside the rectangle
	if(terminate(T,V,depth) || p.getNormal()==-1){
		//std::shared_ptr<KDNode> root = std::make_shared<KDNode>(m_metadata);
		KDNode *root = new KDNode(m_metadata);
		root->left = NULL;
		root->right = NULL;
		root->Tprim = T;
		PH_ASSERT_EQ(root->isLeaf(),1);
		printf("leaf T.size():%lu\n",root->Tprim.tris.size());
		return root;
		//return leaf_node(T)
	}
	
	printf("p normal:%d p getd:%lf\n",p.getNormal(),p.get_d());
	PH_ASSERT_LE(p.getNormal(), 2);
	PH_ASSERT_GE(p.getNormal(), 0);

	Voxel left_voxel;
	Voxel right_voxel;

	//V split by p, use parameters to update left_voxel and right_voxel.
	split_voxel(V, p, left_voxel, right_voxel);

	printf("left_voxel,Min%lf %lf %lf Max:%lf %lf %lf\n right_voxel,Min:%lf %lf %lf Max:%lf %lf %lf\n",left_voxel.box.getMinVertex().x,left_voxel.box.getMinVertex().y,left_voxel.box.getMinVertex().z,left_voxel.box.getMaxVertex().x,left_voxel.box.getMaxVertex().y,left_voxel.box.getMaxVertex().z,right_voxel.box.getMinVertex().x,right_voxel.box.getMinVertex().y,right_voxel.box.getMinVertex().z,right_voxel.box.getMaxVertex().x,right_voxel.box.getMaxVertex().y,right_voxel.box.getMaxVertex().z);
	Triangles left_tris;
	Triangles right_tris;

	//left_tris beyond in left_voxel , so does right_tris.
	left_tris = Union(T , left_voxel);
	right_tris = Union(T , right_voxel);
	//PH_ASSERT_GE(left_tris.tris.size()+right_tris.tris.size(),T.tris.size());
	printf("BUG:tris_size:%lu left_tris_size:%lu, right_tris_size:%lu\n",T.tris.size(),left_tris.tris.size(),right_tris.tris.size());

	//std::shared_ptr<KDNode> root = std::make_shared<KDNode>(m_metadata);
	KDNode *root = new KDNode(m_metadata);
	root->Tprim = T;
	if(left_tris.tris.size() > 0 )
		root->left = recBuild(left_tris, left_voxel, depth+1);
	else
		root->left = NULL;
	if(right_tris.tris.size() > 0 )
		root->right = recBuild(right_tris, right_voxel, depth+1);
	else
		root->right = NULL;
	root->plane = p;
	PH_ASSERT_LE(root->plane.getNormal(), 2);
	PH_ASSERT_GE(root->plane.getNormal(), 0);
	printf("node:%p node plane normal:%d\n",root,root->plane.getNormal());
	//printf("T.size():%lu\n",root->Tprim.tris.size());
	//printf("node:%p node plane normal:%d\n",root.get(),root->plane.getNormal());
	return root;
}

KDNode *KDtree_root;
Voxel World_Voxel;
//std::shared_ptr<KDNode> KDNode::build_KD_tree(Triangles& T){
KDNode *KDNode::build_KD_tree(Triangles& T)
{
	//drawBounds can only call once
	drawBounds(World_Voxel,T);
	//std::shared_ptr<KDNode> temp = recBuild(T,World_Voxel,0);
	KDNode *temp = recBuild(T,World_Voxel,0);
	PH_ASSERT_LE(temp->plane.getNormal(), 2);
	PH_ASSERT_GE(temp->plane.getNormal(), 0);
	printf("Tree root_normal:%d\n",temp->plane.getNormal());
	//KDtree_root = temp.get();
	KDtree_root = temp;
	printf("Tree root_normal:%d\n",KDtree_root->plane.getNormal());
	printf("KDtree_root:%p\n",KDtree_root);
	return temp;
}
//implement virtual functions of primitive.h
//1. implement virtual bool isIntersecting(const Ray& ray, HitProbe& probe) const = 0;
bool KDNode::isIntersecting(const Ray& ray, HitProbe& probe) const {
	//return false;
	//puts("KDNode::isIntersecting called");
	float tMin , tMax;
	bool is_hit = false;
	KDNode *cur_node = KDtree_root;
	if( !World_Voxel.intersect( ray, World_Voxel, &tMin, &tMax) ){
		//puts("KDNode::isIntersecting end");
		return false;
	}
	
	KDQueue queue[MAX_DEPTH];
	int max_todoPos = MAX_DEPTH - 1;
	int todoPos = 0;
	float invDir[3];
	float rayDir[3];
	rayDir[0] = ray.getDirection().x;
	rayDir[1] = ray.getDirection().y;
	rayDir[2] = ray.getDirection().z;
	invDir[0] = 1/ray.getDirection().x;
	invDir[1] = 1/ray.getDirection().y;
	invDir[2] = 1/ray.getDirection().z;
	
	float besthitT;
	Triangle *hitTriangle;
	KDNode *hitNode;
	while(cur_node != NULL){
		if(ray.getMaxT() < tMin)
			break;
		//printf("KDtreeroot:%p cur_node:%p to_doPos=%d\n",KDtree_root,cur_node,todoPos);
		//printf("KDtree normal:%d\n",KDtree_root->plane.getNormal());
		//printf("left:%p right:%p\n",left.get(),right.get());
		//sleep(1);
		if(!cur_node->isLeaf()){
			//puts("not leaf");
			int split_axis = cur_node->plane.getNormal();
			float split_pos = cur_node->plane.get_d();
			//printf("split_axis:%d\n",split_axis);
			float tPlane = (split_pos - ray.getOrigin()[split_axis]) * invDir[split_axis];
			bool left_first = (ray.getOrigin()[split_axis] < split_pos) 
								|| (ray.getOrigin()[split_axis] == split_pos && rayDir[split_axis]<=0);
			//std::shared_ptr<KDNode> cand1;
			//std::shared_ptr<KDNode> cand2;
			KDNode *cand1;
			KDNode *cand2;
			if(left_first){
				// cand1 = cur_node->left.get();
				// cand2 = cur_node->right.get();
				cand1 = cur_node->left;
				cand2 = cur_node->right;
			}
			else{
				//cand1 = cur_node->right.get();
				//cand2 = cur_node->left.get();
				cand1 = cur_node->right;
				cand2 = cur_node->left;
			}
			if (tPlane > tMax || tPlane <= 0)
			{
                cur_node = cand1;
				//printf("cand1:%p\n",cand1);
			}
            else if (tPlane < tMin)
			{
                cur_node = cand2;
				//printf("cand1:%p\n",cand2);
			}
			else { 
                queue[todoPos].node = cand2;
                queue[todoPos].tMin = tPlane;
                queue[todoPos].tMax = tMax;
                ++todoPos;
				assert(todoPos<=max_todoPos);
                cur_node = cand1;
                tMax = tPlane;
            }
		}
		else{
			//puts("leaf");
			//sleep(1);
			//printf("cur_node->Tprim.tris.size():%lu\n",cur_node->Tprim.tris.size());
			for(int i = 0; i < cur_node->Tprim.tris.size(); i++){
				Triangle* triangle = cur_node->Tprim.tris[i];
				float hitT;
				//puts("b1");

				//bug here cant go inside 
				if(triangle->Intersect(ray, &hitT)){
					is_hit = true;
					//probe.pushBaseHit(this, hitT);
					hitTriangle = triangle;
					hitNode = cur_node;
					besthitT = hitT;
					//puts("hit!");
					break;
				}
				//puts("b2");
			}
			if(todoPos > 0 ){
				--todoPos;
				cur_node = queue[todoPos].node;
				tMin = queue[todoPos].tMin;
				tMax = queue[todoPos].tMax;
			}
			else{
				break;
			}
		}

		
	}
	//puts("KDNode::isIntersecting end");
	if(is_hit == true)
	{
		probe.pushBaseHit(hitNode, besthitT);
		probe.cachePointer(hitTriangle);
	}
	return is_hit;
	
}
//2. implement virtual void calcIntersectionDetail(const Ray& ray, HitProbe& probe, HitDetail* out_detail) const = 0
void KDNode::calcIntersectionDetail(const Ray& ray, HitProbe& probe, HitDetail* out_detail) const {

	Triangle* hitTriangle = (Triangle*)(probe.getCachedPointer());
	const Vector3R hitPosition = ray.getOrigin() + ray.getDirection() * probe.getHitRayT();


	out_detail->setMisc(this	, Vector3R(0), probe.getHitRayT());
	out_detail->getHitInfo(ECoordSys::LOCAL).setAttributes(hitPosition, Vector3R(0, 1, 0), Vector3R(0, 1, 0));
	out_detail->getHitInfo(ECoordSys::WORLD) = out_detail->getHitInfo(ECoordSys::LOCAL);
}

//3. accept false positive, tell if two box overlapping.implement bool isIntersectingVolumeConservative(const AABB3D& volume) const = 0;
bool KDNode::isIntersectingVolumeConservative(const AABB3D& volume) const {
	return true;
}
	
//4. pointer send bounding box. implement void calcAABB3D(AABB3D* out_aabb) const = 0;
void KDNode::calcAABB(AABB3D* out_aabb) const {
	//setMinVertex(const Vector3R& minVertex)
	Vector3R myMinVec = World_Voxel.box.getMinVertex();	
	Vector3R myMaxVec = World_Voxel.box.getMaxVertex();
	out_aabb->setMinVertex(Vector3R(myMinVec.x, myMinVec.y, myMinVec.z ));
	out_aabb->setMaxVertex(Vector3R(myMaxVec.x, myMaxVec.y, myMaxVec.z ));
}

}// end namespace ph

