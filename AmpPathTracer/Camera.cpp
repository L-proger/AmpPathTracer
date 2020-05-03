#include "Camera.h"
#include <amp.h>
#include <amp_graphics.h>
#include <amp_math.h>
#include <amprt.h>

using namespace Concurrency::fast_math;


Camera::Camera(void)
	:fov(45.0f), aspect(1.0f)
{

}


Camera::~Camera(void)
{

}

float3 Camera::getCorner(int id) restrict(cpu, amp){
	return corners[id];
}

//0------1
//
//
//2------3
void Camera::ComputeCorners() restrict(cpu){
	float farW = tanf(fov / 2.0f);
	float farH = farW / aspect;
	float3 farCenter = transform.position + transform.forward;
	corners[0] = farCenter + (transform.up * farH) + (transform.right * -farW);
	corners[1] = farCenter + (transform.up * farH) + (transform.right * farW);
	corners[2] = farCenter + (transform.up * -farH) + (transform.right * -farW);
	corners[3] = farCenter + (transform.up * -farH) + (transform.right * farW);
}