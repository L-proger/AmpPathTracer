#ifndef Scene_h__
#define Scene_h__
#include <amp.h>
#include <amp_graphics.h>
#include <amp_math.h>
#include <amprt.h>
#include "AmpTex.h"
#include "AmpMath.h"
#include "IntersectInfo.h"
#include "Camera.h"
#include "Node.h"
#include "ParametricNode.h"
#include "ParametricNodeProc.h"

using namespace Concurrency;
using namespace Concurrency::fast_math;
using namespace Concurrency::graphics;
using namespace Concurrency::graphics::direct3d;
#define TRACE_TS 16

namespace PathTracer{
    class Scene{
    public:
		Scene(int width, int height) restrict(cpu);
        void trace(Tex2d* target2, int frame, int time);
		void clear(Tex2d* target2);
		std::vector<ParametricNode> parametric_nodes;
    private:
		Camera cam;
		std::vector<float4> trace_buffer;
		std::vector<Triangle> triangle_buffer;
		array_view<Triangle, 1>* triangle_buffer_view;
		array_view<float4, 2>* trace_buffer_view;
		Concurrency::array<Triangle, 1>* triangles;
    };
}
#endif // Scene_h__
