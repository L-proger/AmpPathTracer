#ifndef Node_h__
#define Node_h__

#include "Transform.h"

#include <amp.h>
#include <amp_graphics.h>
#include <amp_math.h>
#include <amprt.h>
using namespace Concurrency;
using namespace Concurrency::fast_math;
using namespace Concurrency::graphics;
using namespace Concurrency::graphics::direct3d;
#include "IntersectInfo.h"
#include "Material.h"

enum NodeType
{
	NT_UNDEFINED,
	NT_MESH,
	NT_SPHERE,
	NT_PLANE
};

class Node
{
public:
	Node(void) restrict(cpu, amp);
	~Node(void) restrict(cpu, amp);
	Transform transform;
	Material material;
	NodeType type;
private:
	
};

#endif // Node_h__
