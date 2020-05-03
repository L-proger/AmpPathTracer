#include <amp.h>
#include <amp_graphics.h>
#include <amp_math.h>
#include <amprt.h>
#include <iostream>
#include "Timer.h"

#include "AmpMath.h"
#include "Scene.h"

#include "Dx11Render.h"
using namespace Concurrency;
using namespace Concurrency::fast_math;
using namespace Concurrency::graphics;
using namespace Concurrency::graphics::direct3d;

using namespace PathTracer;



int main(){
	/*ray test_ray;
	test_ray.origin = float3(0,-1,1.0f);
	test_ray.direction = float3(0,1, 0);

	Triangle t1;
	t1.p0 = float3(-1, 0.5f, 0);
	t1.p1 = float3( 0, 0.5f, 1);
	t1.p2 = float3( 1, 0.5f, 0);

	float3 v1 = t1.p1 - t1.p0;
	float3 v2 = t1.p2 - t1.p0;
	t1.normal = normalized(cross(v1, v2));

	float3 pt;
	float2 uv;
	float dist;
	if(RayTriangleIntersect(test_ray, t1, dist, pt, uv)){
		printf("Intersected!\n");
		printf("Coord!  X=%f, Y=%f, Z=%f\n", pt.x, pt.y, pt.z);
		printf("Bary!  X=%f, Y=%f, Z=%f\n", uv.x, uv.y, 1.0f - uv.x - uv.y);
		printf("Distance: %f\n", dist);
	}else{
		printf("Not intersected!\n");
	}

	getchar();*/
    LifeCore::Timer timer;
    Tex2d* tex1;
	Tex2d* tex2;
    LifeCore::RenderWindowWin* window = new LifeCore::RenderWindowWin();
    window->SetSize(800, 600);
    window->CenterWindow();

    Scene scene(800, 600);

	ParametricNode testSphere(NT_SPHERE);
	testSphere.transform.position = float3(0,0,0);
	setSphereRadius(testSphere, 1);
	ray r;
	r.origin = float3(0,0,-2);
	r.direction =  float3(0,0,1);

	IntersectInfo info;
	while(intersect(testSphere, r, info)){
		printf("Intersected\n");
		float eps = -0.00001f;
		if(dot(r.direction, info.normal) > 0.0f){
			info.normal *= -1.0f;
			//eps *= -1.0f;
		}
		r.direction = refract(r.direction, info.normal,1.5f);
		r.origin = info.position + info.normal * eps;
	}


    Dx11Render* renderer = new Dx11Render(window, 800, 600);
    tex1 = renderer->CreateAmpTex(800, 600);
	tex2 = renderer->CreateAmpTex(800, 600);

    bool running = true;
    
    float prevTime = 0.0f;
    float nowTime = 0.0f;
    int iterId = 0;
	int traceId = 0;

	int trace_iter_cnt = 15;
    timer.Start();
    while(running){
		renderer->UnbindRtv();
		renderer->UnbindSrv();

		//for(int i = 0; i < trace_iter_cnt; ++i){
			scene.trace(tex2, traceId, iterId);
			window->Update();
			iterId++;
			traceId++;
		//}
		//scene.clear(tex2);
		//traceId = 0;

        renderer->displayTex(tex2);
        renderer->present();
        //Sleep(10);
        timer.Stop();
        nowTime = timer.getTimeElapsed();
        float timeElapsed = nowTime - prevTime;
        prevTime = nowTime;


        if(!(iterId % 100))
            std::cout << "FPS: " << (1.0f / timeElapsed) << std::endl;
       
    }

    delete renderer;
	
	return 0;
}

