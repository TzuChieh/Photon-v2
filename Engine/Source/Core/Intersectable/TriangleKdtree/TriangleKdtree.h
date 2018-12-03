#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <iostream>
#include <tuple>
#include <float.h>
#include <vector>
#include <assert.h>
#include <limits.h>
#include "Core/Ray.h"
#include "Core/HitProbe.h"
#include "Core/Bound/TAABB3D.h"
#include "Core/Intersectable/Primitive.h"
#include "Math/TVector3.h"
//#include "Core/Intersectable/PTriangle.h"

#define traversal_constant 1
#define intersection_constant 80

#define LEFT 0
#define RIGHT 1

#define BEGIN_EDGE 0
#define END_EDGE 1

#define AXIS_X 0
#define AXIS_Y 1
#define AXIS_Z 2
//normal sutuation inherits Intersector
//inherits primitive

//First build a ez tree
//accelerate by turn Triangle to pointer
//primitive bound make first
//traversal use bit shift
namespace ph
{

using namespace std;

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

float Three_Vec3_delta(const Vec3 A, const Vec3 B, const Vec3 C){
	return A.x * B.y * C.z + B.x * C.y * A.z + C.x * A.y * B.z + C.x * A.y * B.z
			- A.z * B.y * C.z - B.z * C.y * A.x - C.z * A.y * B.x;
}


class AABB{
	private:
		Vec3 VertexMin;
		Vec3 VertexMax;
		bool _is_set = 0;
		bool min_set = 0;
		bool max_set = 0;
	public:
		AABB(){
			_is_set = 0;
			min_set = 0;
			max_set = 0;
		}
		bool is_set(){
			return _is_set;
		}
		void setVertexMin(float x, float y, float z){
			min_set = 1;

			VertexMin.x = x;
			VertexMin.y = y;
			VertexMin.z = z;

			if(max_set==1){
				if(VertexMax.x < VertexMin.x || VertexMax.y < VertexMin.y || VertexMax.z < VertexMin.z){
					
					fprintf(stderr,"VertexMin is not logical, VertexMin somehow is bigger than VertexMax\n");
					printf("VertexMin.x:%f VertexMin.y:%f VertexMin.z:%f\n",VertexMin.x,VertexMin.y,VertexMin.z);
					printf("VertexMax.x:%f VertexMax.y:%f VertexMax.z:%f\n",VertexMax.x,VertexMax.y,VertexMax.z);
					exit(1);
				}
				_is_set=1;
			}
		}
		void setVertexMax(float x, float y, float z){
			max_set = 1;

			VertexMax.x = x;
			VertexMax.y = y;
			VertexMax.z = z;

			if(min_set==1){
				if(VertexMax.x < VertexMin.x || VertexMax.y < VertexMin.y || VertexMax.z < VertexMin.z){
					fprintf(stderr,"VertexMax is not logical, VertexMin somehow is bigger than VertexMax\n");
					exit(1);
				}
				_is_set=1;
			}
		}
		Vec3 getVertexMin(){
			if(_is_set)
				return VertexMin;
			fprintf(stderr,"set AABB box first, use copyVertexMin/Max or set VertexMin/Max\n");
			exit(1);		
		}
		Vec3 getVertexMax(){
			if(_is_set)
				return VertexMax;
			fprintf(stderr,"set AABB box first, use copyVertexMin/Max or set VertexMin/Max\n");
			exit(1);
		}
		void copyVertexMin(AABB src){
			min_set = 1;

			if(src.is_set())
				VertexMin = src.getVertexMin();
			else{
				fprintf(stderr,"the VertexMin/Max of the copied AABB box has not be set\n");
				exit(1);	
			}
			
			if(max_set==1){
				if(VertexMax.x < VertexMin.x || VertexMax.y < VertexMin.y || VertexMax.z < VertexMin.z){
					fprintf(stderr,"VertexMin is not logical, VertexMin somehow is bigger than VertexMax\n");
					exit(1);
				}
				_is_set=1;
			}
		}
		void copyVertexMax(AABB src){
			max_set = 1;
			
			if(src.is_set())
				VertexMax = src.getVertexMax();
			else{
				fprintf(stderr,"the VertexMin/Max of the copied AABB box has not be set\n");
				exit(1);
			}
			if(min_set==1){
				if(VertexMax.x < VertexMin.x || VertexMax.y < VertexMin.y || VertexMax.z < VertexMin.z){
					fprintf(stderr,"VertexMax is not logical, VertexMin somehow is bigger than VertexMax\n");
					exit(1);
				}
				_is_set=1;
			}
		}
};



class BoundEdge{
	private:
		bool perp_is_set = 0;
		bool type_is_set = 0;
		float axis_perp;
		//0 is begin edge, 1 is end edge;
		int EdgeType;
	public:
		BoundEdge(){
			perp_is_set = 0;
			type_is_set = 0;
		}
		BoundEdge(float in_axis_perp ,int in_EdgeType ){
			axis_perp = in_axis_perp;
			EdgeType = in_EdgeType;
			perp_is_set = 1;
			type_is_set = 1;
		}
		void setAxisPerp(float in){
			axis_perp = in;
			perp_is_set = 1;
		}
		float getAxisPerp(){
			if(perp_is_set)
				return axis_perp;
			fprintf(stderr,"setAxisPerp(in) first\n");
			exit(1);
		}

		void setEdgeType(int in){
			EdgeType = in;
			type_is_set = 1;
		}
		int getEdgeType(){
			if(type_is_set)
				return EdgeType;
			fprintf(stderr,"setEdgeType(in) first\n");
			exit(1);
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
			d = -1 * Edge.getAxisPerp();
		}

		void set_d(float in_d){
			d = in_d;
		}
		float get_d(){
			return d;
		}
};

class Triangle{
	private:
	    //get from initilize
		Vec3 vertex[3];
		//get from runtime
		int index = -2;
    public:
		void setIndex(int in){
			index = in;
		}
		int getIndex(){
			if(index==-2){

			}
			fprintf(stderr, "Triangle getIndex err:Triangle verticies does not set, first run setTvertices\n");
			return index;
		}

		AABB TBoundingBox;
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
			Vec3 temp1 = TBoundingBox.getVertexMax();
			Vec3 temp2 = TBoundingBox.getVertexMin();
			if(LongestAxis == AXIS_X){
				return make_tuple(temp2.x,temp1.x);
			}
			else if(LongestAxis == AXIS_Y){
				return make_tuple(temp2.y,temp1.y);
			}
			else if(LongestAxis == AXIS_Z){
				return make_tuple(temp2.z,temp1.z);
			}
			else{
				exit(1);
			}
		}
        void setTvertices(float x1, float y1, float z1, float x2, float y2, float z2,
                            float x3, float y3, float z3){
			index = -1;
            vertex[0].setVec3(x1,y1,z1);
            vertex[1].setVec3(x2,y2,z2);
            vertex[2].setVec3(x3,y3,z3);
        }
		
		Vec3* getTverticies(){
			if(index == -2){
				fprintf(stderr, "Triangle getTverticies err:Triangle verticies does not set, first run setTvertices\n");
				exit(1);
			}
			return vertex;
		}

		bool Intersect(const Ray& ray){
			float o_x = ray.getOrigin().x;
			float o_y = ray.getOrigin().y;
			float o_z = ray.getOrigin().z;
			Vec3 Origin(o_x,o_y,o_z);
			float temp_x = ray.getMaxT() * ray.getDirection().x;
			float temp_y = ray.getMaxT() * ray.getDirection().y;
			float temp_z = ray.getMaxT() * ray.getDirection().z;
			Vec3 ray_vector( temp_x, temp_y ,temp_z);
			//x0 * edgeOA + x1 * edgeOB + x2 * edge OC
			//when x0 + x1 + x2 >= 1 and x0,x1,x2 > 0 has intersect with triangle
			//find the matrix[OA OB OC] whether singular
			// normalize x0,x1,x2 and plus O will find the intersect point 
			Vec3 edgeOA = vertex[0] - Origin;
			Vec3 edgeOB = vertex[1] - Origin;
			Vec3 edgeOC = vertex[2] - Origin;
			//find delta of the matrix
			float delta = Three_Vec3_delta(edgeOA,edgeOB,edgeOC);
			float epsilon = 0.0000001;
			if(delta < epsilon && delta > -epsilon){
				//singular case
				//the ray origin lies on the plane which span by AB and AC
				return false;
			} 

			float deltax_1 = Three_Vec3_delta(ray_vector, edgeOB, edgeOC);
			float deltax_2 = Three_Vec3_delta(edgeOA, ray_vector, edgeOC);
			float deltax_3 = Three_Vec3_delta(edgeOA, edgeOB, ray_vector);
			bool all_positive = (deltax_1 > 0 && deltax_2 > 0 && deltax_3 > 0);	
			if(deltax_1 + deltax_2 + deltax_3 >= 1 && all_positive){
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
class Voxel;
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
		AABB box;
		Voxel(){

		}
		int LongestAxis(){
			if(box.is_set()){		
				Vec3 temp = box.getVertexMax() - box.getVertexMin();
				if(temp.x > temp.y && temp.x > temp.z){
					return AXIS_X;
				}
				else if(temp.y > temp.x && temp.y > temp.z){
					return AXIS_Y;
				}
				else{
					return AXIS_Z;
				}
			}

			fprintf(stderr,"set AABB box of Voxel first, use copyVertexMin/Max or set VertexMin/Max");
			exit(1);
		}
		bool intersect(const Ray& ray, Voxel World_Voxel,float *outTMin, float *outTMax){
			//base by pbrt			
			float t0 = 0;
			float t1 = ray.getMaxT();
			Vector3R temp = ray.getDirection();
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
			boxMin[0] = World_Voxel.box.getVertexMin().x;
			boxMin[1] = World_Voxel.box.getVertexMin().y;
			boxMin[2] = World_Voxel.box.getVertexMin().z;

			boxMax[0] = World_Voxel.box.getVertexMax().x;
			boxMax[1] = World_Voxel.box.getVertexMax().y;
			boxMax[2] = World_Voxel.box.getVertexMax().z;						

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


typedef struct s{
	KDNode *node;
    float tMin, tMax;
}KDQueue;

std::tuple<float,float,float,float,float,float> TriangleBound(Triangle *t, int index);
void drawBounds(Voxel& V, Triangles& T);

bool PointInAABB(Vec3 Point, AABB& Box);
bool TriangleInAABB(Triangle* tri, AABB& Box);

Triangles Union(Triangles& T, Voxel& V);

float Cost(float K_t, float K_i, float P_left, float P_right, int left_traingles_n, int right_traingles_n);
float SA(Voxel& V);

void split_voxel(Voxel& V,Plane& P, Voxel& left_voxel, Voxel& right_voxel);
float SAH(Plane& p, Voxel& V, int left_traingles_n, int right_traingles_n);
void setTriBoundingEdge(Triangles& T, int LongestAxis, vector<BoundEdge>& Edge);
bool terminate(Triangles& T, Voxel& V, int depth);
bool edgeCmp(BoundEdge a, BoundEdge b);
Plane find_plane(Triangles& T, Voxel& V);

}// end namespace ph
