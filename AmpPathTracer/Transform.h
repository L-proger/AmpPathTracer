#ifndef Transform_h__
#define Transform_h__

#include "AmpMath.h"

class Transform
{
public:
	Transform(void) restrict(cpu, amp);
	~Transform(void) restrict(cpu, amp);
	
	float3 right;
	float3 up;
	float3 forward;
	float3 position;
};

#endif // Transform_h__
