#include "Transform.h"


Transform::Transform(void) restrict(cpu, amp)
	:position(0,0,0), right(1,0,0), up(0,1,0), forward(0,0,1)
{
}


Transform::~Transform(void) restrict(cpu, amp)
{
}
