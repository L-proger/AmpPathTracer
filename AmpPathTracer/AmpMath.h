#ifndef AmpMath_h__
#define AmpMath_h__
#include <amp.h>
#include <amp_graphics.h>
#include <amp_math.h>
#include <amprt.h>

using namespace Concurrency::graphics::direct3d;
using namespace Concurrency::fast_math;

/*#ifdef max
#undef max
#endif

#ifdef min
#undef min
#endif*/

struct ray{
public:
	ray(const float3& orig, const float3& dir) restrict(cpu, amp)
		:origin(orig), direction(dir)
	{

	}
	ray() restrict(cpu, amp){

	}
	float3 origin;
	float3 direction;
};



inline float3 reflect(const float3& normal, const float3& incident) restrict(cpu, amp)
{
	float cosine = normal.x * incident.x + normal.y * incident.y + normal.z * incident.z;
	return incident - normal * (2.0f * cosine);
}


inline float length(const float3& vec) restrict(cpu, amp){
    return sqrtf(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
}

inline float lengthsq(const float3& vec) restrict(cpu, amp){
	return vec.x * vec.x + vec.y * vec.y + vec.z * vec.z;
}

inline float dot(const float3& v1, const float3& v2) restrict(amp, cpu){
	return (v1.x * v2.x + v1.y * v2.y + v1.z * v2.z);
}

inline float dot(const float2& v1, const float2& v2) restrict(amp, cpu){
	return (v1.x * v2.x + v1.y * v2.y);
}

inline float3 lerp(const float3& vector1, const float3& vector2, float val) restrict(amp, cpu)
{
	return vector1 * (1.0f - val) + vector2 * val;
}

#define cross_v3(vector1, vector2, v_result) \
	v_result.x = (vector1.y * vector2.z) - (vector1.z * vector2.y); \
	v_result.y = (vector1.z * vector2.x) - (vector1.x * vector2.z); \
	v_result.z = (vector1.x * vector2.y) - (vector1.y * vector2.x);

inline float3 cross(const float3& vector1, const float3& vector2) restrict(amp, cpu)
{
	float3 vector;
	vector.x = (vector1.y * vector2.z) - (vector1.z * vector2.y);
	vector.y = (vector1.z * vector2.x) - (vector1.x * vector2.z);
	vector.z = (vector1.x * vector2.y) - (vector1.y * vector2.x);
	return vector;
}

inline float3 normalized(const float3& vec) restrict(cpu, amp){
	float len = length(vec);
	return float3(vec.x / len, vec.y / len, vec.z / len);
}


inline void normalize(float3& vec) restrict(cpu, amp){
	float rcp_len = 1.0f / length(vec);
	vec.x *= rcp_len;
	vec.y *= rcp_len;
	vec.z *= rcp_len;
}

inline float length(const float2& vec) restrict(amp, cpu){
    return sqrtf(vec.x * vec.x + vec.y * vec.y);
}

/*inline float random(const float3& scale, float seed, const float3& coord)  restrict(cpu, amp)
{
	float val = (sinf(seed + dot(coord, scale)) * 43758.5453f);
	return val - float((int)val);
}*/
inline float frac(float val) restrict(amp) {
	return val - (float)(int(floorf(val)));
}


inline float random(const float3& scale, float seed, const float3& coord) restrict(amp) {
	return frac(sin(dot(coord + seed, scale)) * 43758.5453f + seed);
}

inline float3 refract(const float3 &incidentVec, const float3 &normal, float eta) restrict(cpu, amp)
{
	float N_dot_I = dot(normal, incidentVec);
	float k = 1.f - eta * eta * (1.f - N_dot_I * N_dot_I);
	float cosTheta2 = sqrt(k);

	if (k < 0.f)
		return float3(0.f, 0.f, 0.f);
	else{
		return eta * incidentVec - (eta * N_dot_I + sqrtf(k)) * normal;
	}
}

/*__host__ __device__
	float3 computeTransmissionDirection(const float3 & normal, const float3 & incident, float refractiveIndexIncident, float refractiveIndexTransmitted) {
		// Snell's Law:
		// Copied from Photorealizer.

		float cosTheta1 = dot(normal, incident);

		float n1_n2 =  refractiveIndexIncident /  refractiveIndexTransmitted;

		float radicand = 1 - pow(n1_n2, 2) * (1 - pow(cosTheta1, 2));
		if (radicand < 0) return make_float3(0, 0, 0); // Return value???????????????????????????????????????
		float cosTheta2 = sqrt(radicand);

		if (cosTheta1 > 0) { // normal and incident are on same side of the surface.
			return n1_n2 * (-1 * incident) + ( n1_n2 * cosTheta1 - cosTheta2 ) * normal;
		} else { // normal and incident are on opposite sides of the surface.
			return n1_n2 * (-1 * incident) + ( n1_n2 * cosTheta1 + cosTheta2 ) * normal;
		}

}*/

/*inline float3 FromSpherical(float theta, float phi) restrict(cpu, amp)
{
	float fSinTheta = sin(theta);
	return float3( fSinTheta * cosf(phi), fSinTheta * sinf(phi), cosf(theta) );
}


inline float3 ToSpherical(const float3 vec) restrict(cpu, amp)
{
	float3 normalized_v = normalized(vec);
	return float3(acosf(vec.z), atan2f(vec.y, vec.x), 1.0f);
}*/


inline float3 cosineWeightedDirection(float seed, const float3& normal, const float3& coord) restrict(amp) 
{
	float u = random(float3(12.9898f, 78.233f, 151.7182f), seed, coord);
	float v = random(float3(63.7264f, 10.873f, 623.6736f), seed, coord);
	float r = sqrtf(u);
	float angle = 6.283185307179586f * v;
	// compute basis from normal
	float3 sdir, tdir;
	if (fabsf(normal.x)<0.5f) {
		sdir = cross(normal, float3(1.0f,0.0f,0.0f));
	} else {
		sdir = cross(normal, float3(0.0f,1.0f,0.0f));
	}
	cross_v3(normal, sdir, tdir);
	return r*cosf(angle)*sdir + r*sinf(angle)*tdir + sqrtf(1.0f-u)*normal;
}


//cosineWeightedDirection(timeSinceStart + float(bounce), normal);';


/*bool Intersect(const ray r) restrict(amp){
	float3 rayDir = normalized(r.direction);
	float3 v = r.origin - this->GetOrigin();
	float b = 2.0f * Vec3Dot(&rayDir, &v);
	float c = Vec3Dot(&v, &v) - radius*radius;
	float D = b*b - (4.0f * c);

	if(D < 0.0f)
		return false;

	D = sqrtf(D);

	float s0 = (-b + D) / 2.0f;
	float s1 = (-b - D) / 2.0f;


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
		info->Position = ray.Origin + ray.Direction * dist;
		info->Normal = info->Position - this->GetOrigin();
		info->Distance = dist;
		info->material = material;
		info->Color = material->Emittence;
		Vec3Normalize(&info->Normal);
		return true;
	}
	return false;
}*/


struct Triangle{
	float3 p0;
	float3 p1;
	float3 p2;
	float3 normal;

	float3 v10(){
		return p1 - p0;
	}
	float3 v20(){
		return p2 - p0;
	}
	void computeNormal(){
		normal = normalized(cross(v10(), v20()));
	}
};

inline float2 barycentricCoordinates(const float3& p, const Triangle& t) restrict(cpu, amp)
{
	float3 v0 = t.p2 - t.p0;
	float3 v1 = t.p1 - t.p0;
	float3 v2 = p - t.p0;
	float dot00 = dot(v0, v0);
	float dot01 = dot(v0, v1);
	float dot02 = dot(v0, v2);
	float dot11 = dot(v1, v1);
	float dot12 = dot(v1, v2);
	float invDenom = 1.0f / (dot00 * dot11 - dot01 * dot01);
	return float2((dot11 * dot02 - dot01 * dot12) * invDenom, (dot00 * dot12 - dot01 * dot02) * invDenom);
}


inline bool RayTriangleIntersect(ray& r, Triangle& t, float& distance, float3& point, float2& uv){
	float N_dot_RD = dot(t.normal, r.direction); //ray direction projection on plane
	if(N_dot_RD == 0.0f){ //no intersection, ray is parallel to plane
		return false;
	}

	//find distance from ray origin to plane
	float N_dot_P = dot(t.normal, (t.p0 - r.origin));
	//find intersection distance along ray
	distance = (N_dot_P / N_dot_RD);
	if(distance < 0.0f){
		return false;
	}

	point = r.origin + r.direction * distance;
	uv = barycentricCoordinates(point, t);
	return (uv.x >= 0.0f) && (uv.y >= 0.0f) && (uv.x + uv.y <= 1.0f);
}

#endif // AmpMath_h__
