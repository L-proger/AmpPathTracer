#ifndef ParametricNodeProc_h__
#define ParametricNodeProc_h__

#include "ParametricNode.h"
#include "AmpMath.h"

inline bool intersectSphere(const ParametricNode& node, const ray& r) restrict(amp){
	float3 v = r.origin - node.transform.position;
	float b = 2.0f * dot(r.direction, v);
	float radius = getSphereRadius(node);
	float c = dot(v, v) - radius*radius;
	float D = b*b - (4.0f * c);

	if(D < 0.0f)
		return false;

	D = sqrtf(D);

	float s0 = (-b + D) * 0.5f;
	float s1 = (-b - D) * 0.5f;


	float dist;
	bool hit = false;

	if(s0>0.0f)
	{
		dist = s0;
		hit = true;
	}

	if(s1>0.0f)
	{
		if(dist > s1)
			dist = s1;	
		hit = true;
	}
	
	return hit;
}

inline bool RayTriangleIntersect(ray& r, Triangle& t, IntersectInfoBasic& out_info) restrict(cpu, amp){
	float N_dot_RD = dot(t.normal, r.direction); //ray direction projection on plane
	if(N_dot_RD == 0.0f){ //no intersection, ray is parallel to plane
		return false;
	}

	//find distance from ray origin to plane
	float N_dot_P = dot(t.normal, (t.p0 - r.origin));
	//find intersection distance along ray
	out_info.distance = (N_dot_P / N_dot_RD);
	if(out_info.distance < 0.0f){
		return false;
	}

	out_info.position = r.origin + r.direction * out_info.distance;
	out_info.uv = barycentricCoordinates(out_info.position, t);
	out_info.normal = t.normal;
	return (out_info.uv.x >= 0.0f) && (out_info.uv.y >= 0.0f) && (out_info.uv.x + out_info.uv.y <= 1.0f);
}


inline bool intersect_basic(const ParametricNode& node, const ray& r, IntersectInfoBasic& out_info) restrict(cpu, amp){
	switch (node.type)
	{
	case NT_SPHERE:
		{
			float3 v = r.origin - node.transform.position;
			float b = 2.0f * dot(r.direction, v);
			float radius = getSphereRadius(node);
			float c = dot(v, v) - radius*radius;
			float D = b*b - (4.0f * c);

			if(D < 0.0f)
				return false;

			D = sqrtf(D);

			float s0 = (-b + D) * 0.5f;
			float s1 = (-b - D) * 0.5f;


			float dist;
			bool hit = false;

			if(s0>0.0f)
			{
				dist = s0;
				hit = true;
			}

			if(s1>0.0f)
			{
				if(dist > s1)
					dist = s1;	
				hit = true;
			}
			if(hit)
			{
				out_info.position = r.origin + r.direction * dist;
				out_info.normal = normalized(out_info.position - node.transform.position);
				out_info.distance = dist;
				return true;
			}
			return hit;
		}
		break;
	case NT_PLANE:
		{
			float3 normal = getPlane(node).xyz;
			float dotProduct = dot(normal, r.direction);
			if(dotProduct>=0)
				return false;

			float3 rayToPlaneDir = node.transform.position - r.origin;
			float det = dot(normal, rayToPlaneDir) / dotProduct;

			if (det < 0)
				return false;

			out_info.normal = normal;
			out_info.position = r.origin + r.direction * det;
			out_info.distance = det;
			return true;
		}
		break;
	}
	return false;
}

inline bool intersect(const ParametricNode& node, const ray& r, IntersectInfo& out_info) restrict(cpu, amp){
	switch (node.type)
	{
	case NT_SPHERE:
		{
			float3 v = r.origin - node.transform.position;
			float b = 2.0f * dot(r.direction, v);
			float radius = getSphereRadius(node);
			float c = dot(v, v) - radius*radius;
			float D = b*b - (4.0f * c);

			if(D < 0.0f)
				return false;

			D = sqrtf(D);

			float s0 = (-b + D) * 0.5f;
			float s1 = (-b - D) * 0.5f;


			float dist;
			bool hit = false;

			if(s0>0.0f)
			{
				dist = s0;
				hit = true;
			}

			if(s1>0.0f)
			{
				if(dist > s1)
					dist = s1;	
				hit = true;
			}
			if(hit)
			{
				out_info.position = r.origin + r.direction * dist;
				out_info.normal = normalized(out_info.position - node.transform.position);
				out_info.mat = node.material;
				out_info.distance = dist;
				return true;
			}
			return hit;
		}
		break;
	case NT_PLANE:
		{
			float3 normal = getPlane(node).xyz;
			float dotProduct = dot(normal, r.direction);
			if(dotProduct>=0)
				return false;

			float3 rayToPlaneDir = node.transform.position - r.origin;
			float det = dot(normal, rayToPlaneDir) / dotProduct;

			if (det < 0)
				return false;

			out_info.normal = normal;
			out_info.position = r.origin + r.direction * det;
			out_info.mat = node.material;
			out_info.distance = det;
			return true;
		}
		break;
	}
	return false;
}
#endif // ParametricNodeProc_h__
