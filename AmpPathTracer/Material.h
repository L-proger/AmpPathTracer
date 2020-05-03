#ifndef Material_h__
#define Material_h__

#include "AmpMath.h"

enum ReflectionFunc{
	R_CONST,
	R_FRESNEL
};

struct Material{
	Material()restrict(cpu, amp)
		:diffuse(1,1,1), emissive(0,0,0), roughness(1.0f), glossiness(0), refraction(0)
	{

	}
	float refraction;
	float roughness;
	float3 diffuse;
	float glossiness;
	float3 emissive;
};
#endif // Material_h__
