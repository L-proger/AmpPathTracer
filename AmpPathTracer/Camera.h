#ifndef Camera_h__
#define Camera_h__

#include "Transform.h"

class Camera
{
public:
	Camera(void);
	~Camera(void);
	Transform transform;
	void ComputeCorners() restrict(cpu);
	float3 getCorner(int id) restrict(cpu, amp);
	float aspect;
	float fov;
private:
	float3 corners[4];
	
	
};

#endif // Camera_h__
