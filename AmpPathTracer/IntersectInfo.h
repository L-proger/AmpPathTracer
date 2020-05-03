#ifndef IntersectInfo_h__
#define IntersectInfo_h__

#include "AmpMath.h"
#include "Material.h"

struct IntersectInfo{
	float3 position;
	float distance;
	float3 normal;
	Material mat;
};

struct IntersectInfoBasic{
	float3 position;
	float distance;
	float3 normal;
	float2 uv;
};

struct IntersectInfoTriangle{
	float3 position;
	float distance;
	float2 uv;
};
#endif // IntersectInfo_h__
