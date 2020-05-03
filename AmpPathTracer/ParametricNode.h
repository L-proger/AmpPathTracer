#ifndef ParametricNode_h__
#define ParametricNode_h__

#include "Node.h"

class ParametricNode : public Node{
public:
	ParametricNode(NodeType t) restrict(cpu, amp);
	/*void setPlane(float4 plane) restrict(cpu, amp);
	float4 getPlane() const restrict(cpu, amp);
	void setSphereRadius(float radius) restrict(cpu, amp);
	float getSphereRadius() const restrict(cpu, amp);*/
	float4 data0;
};

inline void setPlane(ParametricNode& node, float4 plane) restrict(cpu, amp){
	node.data0 = plane;
}
inline float4 getPlane(const ParametricNode& node) restrict(cpu, amp){
	return node.data0;
}
inline void setSphereRadius(ParametricNode& node, float radius) restrict(cpu, amp){
	node.data0.x = radius;
}
inline float getSphereRadius(const ParametricNode& node) restrict(cpu, amp){
	return node.data0.x;
}
#endif // ParametricNode_h__
