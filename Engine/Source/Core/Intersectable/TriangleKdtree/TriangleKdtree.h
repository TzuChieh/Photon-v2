#include "Core/Ray.h"
#include "Core/HitProbe.h"
#include "Core/Bound/TAABB3D.h"
#include "Core/Intersectable/Primitive.h"
#include "Math/math_fwd.h"
#include "Math/math.h"
#include "Common/assertion.h"
#include "Core/Intersectable/PTriangle.h"
//#include "Core/Intersectable/PTriangle.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <iostream>
#include <tuple>
#include <float.h>
#include <vector>
#include <assert.h>
#include <limits.h>

constexpr int traversal_constant = 1;
constexpr int intersection_constant = 80;

constexpr int LEFT = 0;
constexpr int RIGHT = 1;

constexpr int BEGIN_EDGE = 0;
constexpr int END_EDGE = 1;

//normal sutuation inherits Intersector
//inherits primitive

//First build a ez tree
//accelerate by turn Triangle to pointer
//primitive bound make first
//traversal use bit shift

namespace ph
{
class KDNode;
class Voxel;
/*
class Vec3 {
	public:
		float x;
		float y;
		float z;
		//friend is needed, without friend there will be an implicit parameters this which cause compile error
		friend Vec3 operator-(Vec3 lhs, const Vec3& rhs){
			Vec3 temp;
			temp.x = lhs.x - rhs.x;
			temp.y = lhs.y - rhs.y;
			temp.z = lhs.z - rhs.z;
			return temp;
		}

		Vec3(){

		}

		Vec3(float in_x, float in_y, float in_z){
			x = in_x;
			y = in_y;
			z = in_z;
		}

		void setVec3(float in_x, float in_y, float in_z){
			x = in_x;
			y = in_y;
			z = in_z;
		}
};
*/
inline float Three_Vec3_delta(const Vector3F& A, const Vector3F& B, const Vector3F& C){
	return A.x * B.y * C.z + B.x * C.y * A.z + C.x * A.y * B.z + C.x * A.y * B.z
			- A.z * B.y * C.z - B.z * C.y * A.x - C.z * A.y * B.x;
}

/*
class AABB3D{
	private:
		Vector3F VertexMin;
		Vector3F VertexMax;
	public:
		AABB3D(){

		}
		void setVertexMin(float x, float y, float z){

			VertexMin.x = x;
			VertexMin.y = y;
			VertexMin.z = z;

			if(VertexMax.x < VertexMin.x || VertexMax.y < VertexMin.y || VertexMax.z < VertexMin.z){
					
				fprintf(stderr,"VertexMin is not logical, VertexMin somehow is bigger than VertexMax\n");
				printf("VertexMin.x:%f VertexMin.y:%f VertexMin.z:%f\n",VertexMin.x,VertexMin.y,VertexMin.z);
				printf("VertexMax.x:%f VertexMax.y:%f VertexMax.z:%f\n",VertexMax.x,VertexMax.y,VertexMax.z);
				exit(1);
			}

		}
		void setVertexMax(float x, float y, float z){

			VertexMax.x = x;
			VertexMax.y = y;
			VertexMax.z = z;

			if(VertexMax.x < VertexMin.x || VertexMax.y < VertexMin.y || VertexMax.z < VertexMin.z){
				fprintf(stderr,"VertexMax is not logical, VertexMin somehow is bigger than VertexMax\n");
				exit(1);
			}

		}
		Vector3F getMinVertex(){
			return VertexMin;		
		}
		Vector3F getMaxVertex(){
			return VertexMax;
		}

		void copyVertexMin(AABB3D src){
			min_set = true;

			if(src.is_set())
				VertexMin = src.getMinVertex();
			else{
				fprintf(stderr,"the VertexMin/Max of the copied AABB3D box has not be set\n");
				exit(1);	
			}
			
			if(max_set == true){
				if(VertexMax.x < VertexMin.x || VertexMax.y < VertexMin.y || VertexMax.z < VertexMin.z){
					fprintf(stderr,"VertexMin is not logical, VertexMin somehow is bigger than VertexMax\n");
					exit(1);
				}
				_is_set = true;
			}
		}
		void copyVertexMax(AABB3D src){
			max_set = true;
			
			if(src.is_set())
				VertexMax = src.getMaxVertex();
			else{
				fprintf(stderr,"the VertexMin/Max of the copied AABB3D box has not be set\n");
				exit(1);
			}
			if(min_set == true){
				if(VertexMax.x < VertexMin.x || VertexMax.y < VertexMin.y || VertexMax.z < VertexMin.z){
					fprintf(stderr,"VertexMax is not logical, VertexMin somehow is bigger than VertexMax\n");
					exit(1);
				}
				_is_set = true;
			}
		}

};
*/


class BoundEdge{
	private:
		float split_pos;
		//0 is begin edge, 1 is end edge;
		int EdgeType;
	public:
		BoundEdge(){
			split_pos = 0;
			EdgeType = -1;
		}
		BoundEdge(float in_split_pos ,int in_EdgeType ){
			split_pos = in_split_pos;
			EdgeType = in_EdgeType;
		}
		void setSplitPos(float in){
			split_pos = in;
		}
		float getSplitPos(){
			PH_ASSERT_NE(EdgeType, -1);
			return split_pos;
		}

		void setEdgeType(int in){
			EdgeType = in;
		}
		int getEdgeType(){
			PH_ASSERT_NE(EdgeType, -1);
			return EdgeType;
		}
};

class Plane{
	//Use 2 points to refer a plane with margin
	private:
		int Normal;
		float d;
	public:
		//margin
		//0 point to x, 1 point to y, 2 point to z
		//ax + by + cz  = d if normal is  (1,0,0),need to know the d
		
		Plane(){
			Normal = -1;
		}
		/*
		void setNormal(int direction){
			Normal = (char)direction;
		}
		*/
		void setNormal(int direction){
			Normal = direction;
		}
		int getNormal(){
			if(Normal == -1){
				fprintf(stderr,"Plane d does not initilize, use constructor Plane(BoundEdge Edge, int LongestAxis) or Plane.set_d()\n");
				exit(1);
			}
			return ((Normal>>1) == 1)? 2 : Normal;
		}

		Plane(BoundEdge Edge, int LongestAxis ){
			Normal = LongestAxis;
			d = -1 * Edge.getSplitPos();
		}

		void set_d(float in_d){
			d = in_d;
		}
		float get_d(){
			return d;
		}
};

class Triangle {
	private:
	    //get from initilize

		//bug? TVector3.ipp
		//Vector3F vertex[3];
		//get from runtime
		Vector3F vertexA;
		Vector3F vertexB;
		Vector3F vertexC;
		int index = -2;
    public:
		void setIndex(int in){
			index = in;
		}
		int getIndex(){
			if(index==-2){
				fprintf(stderr, "Triangle getIndex err:Triangle verticies does not set, first run setTvertices\n");
			}	
			return index;
		}
		
		AABB3D TBoundingBox;
		std::tuple<float, float> getBoundingEdge(int LongestAxis){
			switch(index){
				case -2:
					fprintf(stderr, "Triangle getBoundingEdge err:TBoundingBox does not set, first run drawBounds\n");
					exit(1);
					break;
				case -1:
					fprintf(stderr, "Triangle getBoundingEdge err:TBoundingBox has set, but no given index\n");
					exit(1);
					break;
			}
			Vector3F temp1 = TBoundingBox.getMaxVertex();
			Vector3F temp2 = TBoundingBox.getMinVertex();
			if(LongestAxis == math::X_AXIS){
				return std::make_tuple(temp2.x,temp1.x);
			}
			else if(LongestAxis == math::Y_AXIS){
				return std::make_tuple(temp2.y,temp1.y);
			}
			else if(LongestAxis == math::Z_AXIS){
				return std::make_tuple(temp2.z,temp1.z);
			}
			else{
				exit(1);
			}
		}
        void setTvertices(float x1, float y1, float z1, float x2, float y2, float z2,
                            float x3, float y3, float z3){
			index = -1;
			/*
            vertex[0].set(x1,y1,z1);
            vertex[1].set(x2,y2,z2);
            vertex[2].set(x3,y3,z3);
			*/
			vertexA.set(x1,y1,z1);
            vertexB.set(x2,y2,z2);
            vertexC.set(x3,y3,z3);
        }
		/*
		Vector3F* getTverticies(){
			if(index == -2){
				fprintf(stderr, "Triangle getTverticies err:Triangle verticies does not set, first run setTvertices\n");
				exit(1);
			}
			return vertex;
		}
		*/
		Vector3R& getVertexA()
		{
			return vertexA;
		}

		Vector3R& getVertexB()
		{
			return vertexB;
		}

		Vector3R& getVertexC()
		{
			return vertexC;
		}

		bool Intersect(const Ray& ray, float *out_t){
			float o_x = ray.getOrigin().x;
			float o_y = ray.getOrigin().y;
			float o_z = ray.getOrigin().z;
			Vector3F Origin(o_x,o_y,o_z);
			float temp_x = ray.getMaxT() * ray.getDirection().x;
			float temp_y = ray.getMaxT() * ray.getDirection().y;
			float temp_z = ray.getMaxT() * ray.getDirection().z;
			Vector3F ray_vector( temp_x, temp_y ,temp_z);
			//x0 * edgeOA + x1 * edgeOB + x2 * edge OC
			//when x0 + x1 + x2 >= 1 and x0,x1,x2 > 0 has intersect with triangle
			//find the matrix[OA OB OC] whether singular
			// normalize x0,x1,x2 and plus O will find the intersect point 
			/*
			Vector3F edgeOA = vertex[0] - Origin;
			Vector3F edgeOB = vertex[1] - Origin;
			Vector3F edgeOC = vertex[2] - Origin;
			*/
			Vector3F edgeOA = vertexA - Origin;
			Vector3F edgeOB = vertexB - Origin;
			Vector3F edgeOC = vertexC - Origin;
			//find delta of the matrix
			float delta = Three_Vec3_delta(edgeOA,edgeOB,edgeOC);
			float epsilon = 0.0000001;
			if(delta < epsilon && delta > -epsilon){
				//singular case
				//the ray origin lies on the plane which span by AB and AC
				return false;
			} 
			float inv_delta = 1.0/delta;
			float deltax_1 = Three_Vec3_delta(ray_vector, edgeOB, edgeOC);
			float deltax_2 = Three_Vec3_delta(edgeOA, ray_vector, edgeOC);
			float deltax_3 = Three_Vec3_delta(edgeOA, edgeOB, ray_vector);
			bool all_positive = (deltax_1 > 0 && deltax_2 > 0 && deltax_3 > 0);	
			if((deltax_1 + deltax_2 + deltax_3) * inv_delta >= 1 && all_positive){
				float inv_temp = 1.0/ ((deltax_1 + deltax_2 + deltax_3) * inv_delta);
				*out_t = ray.getMaxT() * inv_temp;
				return true;
			}

			return false;

 		}
		
};

class Triangles{
	public:
		std::vector<Triangle*> tris;
		Triangles(){
			
		}
		//move	
		Triangles(Triangles&& other) {
			//tris.swap(other.tris);
			tris = std::move(other.tris);
		}		
		//implement initilize list
		Triangles& operator=(Triangles&& other) {
			//tris.swap(other.tris);
			tris = std::move(other.tris);
			return *this;
		}

		//copy
		Triangles(const Triangles& other) {
			//tris.swap(other.tris);
			tris = other.tris;
		}		

		Triangles& operator=(const Triangles& other) {
			//tris.swap(other.tris);
			tris = other.tris;
			return *this;
		}
		
};

class KDNode: public Primitive{
	public:
		KDNode *left;
		KDNode *right;
		Triangles Tprim;
		Plane plane;
   		KDNode(const PrimitiveMetadata* metadata) : Primitive(metadata)
    	{ 
        	left = NULL; 
        	right = NULL; 
    	} 
		bool isLeaf(){
			return (left==NULL && right==NULL);
		}
		
		bool isIntersecting(const Ray& ray, HitProbe& probe) const override;
		void calcIntersectionDetail(const Ray& ray, HitProbe& probe, HitDetail* out_detail)  const override;
		bool isIntersectingVolumeConservative(const AABB3D& volume) const override;
		void calcAABB(AABB3D* out_aabb) const override;
		KDNode* recBuild(Triangles& T, Voxel& V, int depth);
		KDNode* build_KD_tree(Triangles& T);
};
class Voxel{
	public:
		AABB3D box;
		Voxel(){

		}
		int LongestAxis(){	
			Vector3F temp = box.getMaxVertex() - box.getMinVertex();
			if(temp.x > temp.y && temp.x > temp.z){
				return math::X_AXIS;
			}
			else if(temp.y > temp.x && temp.y > temp.z){
				return math::Y_AXIS;
			}
			else{
				return math::Z_AXIS;
			}
		}
		bool intersect(const Ray& ray, Voxel World_Voxel,float *outTMin, float *outTMax){
			//base by pbrt			
			float t0 = 0;
			float t1 = ray.getMaxT();
			Vector3F temp = ray.getDirection();
			float dir[3];
			dir[0] = temp.x;
			dir[1] = temp.y;
			dir[2] = temp.z;
			
			temp = ray.getOrigin();
			float origin[3];
			origin[0] = temp.x;
			origin[1] = temp.y;
			origin[2] = temp.z;

			float boxMax[3];
			float boxMin[3];
			boxMin[0] = World_Voxel.box.getMinVertex().x;
			boxMin[1] = World_Voxel.box.getMinVertex().y;
			boxMin[2] = World_Voxel.box.getMinVertex().z;

			boxMax[0] = World_Voxel.box.getMaxVertex().x;
			boxMax[1] = World_Voxel.box.getMaxVertex().y;
			boxMax[2] = World_Voxel.box.getMaxVertex().z;						

			for(int i = 0; i < 3; i ++){
				float inv = 1/dir[i];
				float tNear = inv * (boxMin[i] - origin[i]);
				float tFar = inv * (boxMax[i] - origin[i]);
				if(tNear>tFar){
					std::swap(tNear,tFar);
				}
			
				t0 = tNear > t0 ? tNear : t0;
   				t1 = tFar  < t1 ? tFar  : t1;
   				if (t0 > t1) return false;
			}
			 if(outTMin) *outTMin = t0;
			 if(outTMax) *outTMax = t1;
			 return true;
		}
};


std::tuple<float,float,float,float,float,float> TriangleBound(Triangle *t, int index);
void drawBounds(Voxel& V, Triangles& T);

bool PointInAABB3D(Vector3F Point, AABB3D& Box);
bool TriangleInAABB3D(Triangle* tri, AABB3D& Box);

Triangles Union(Triangles& T, Voxel& V);

float Cost(float K_t, float K_i, float P_left, float P_right, int left_traingles_n, int right_traingles_n);
float SA(Voxel& V);

void split_voxel(Voxel& V,Plane& P, Voxel& left_voxel, Voxel& right_voxel);
float SAH(Plane& p, Voxel& V, int left_traingles_n, int right_traingles_n);
void setTriBoundingEdge(Triangles& T, int LongestAxis, std::vector<BoundEdge>& Edge);
bool terminate(Triangles& T, Voxel& V, int depth);
bool edgeCmp(BoundEdge a, BoundEdge b);
Plane find_plane(Triangles& T, Voxel& V);

}// end namespace ph
