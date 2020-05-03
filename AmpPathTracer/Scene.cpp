#include "Scene.h"
#include "Node.h"
#include "ParametricNodeProc.h"
#include <numeric>

namespace PathTracer{

    Scene::Scene(int width, int height) restrict(cpu){
		trace_buffer.resize(width * height);

		trace_buffer_view = new array_view<float4, 2>(height, width, trace_buffer);

		Triangle tri;
		tri.p0 = float3(-1, 0.1f, 3);
		tri.p1 = float3( 0, 1.1f, 3);
		tri.p2 = float3( 1, 0.1f, 3);
		tri.computeNormal();

		triangle_buffer.push_back(tri);

		triangle_buffer_view = new array_view<Triangle, 1>(triangle_buffer.size(), triangle_buffer);

		ParametricNode plane(NT_PLANE);
		setPlane(plane, float4(0,1,0,0));
		plane.transform.position = float3(0,0,0);
		plane.material.emissive = float3(0,0,0);

		ParametricNode ceiling(NT_PLANE);
		setPlane(ceiling, float4(0,-1,0,0));
		ceiling.transform.position = float3(0,2,0);
		ceiling.material.emissive = float3(0,0,0);
		ceiling.material.diffuse = float3(1,1,1);

		ParametricNode farWall(NT_PLANE);
		setPlane(farWall, float4(0,0,-1,0));
		farWall.transform.position = float3(0,0,8);
		farWall.material.emissive = float3(0,0,0);

		ParametricNode nearWall(NT_PLANE);
		setPlane(nearWall, float4(0,0,1,0));
		nearWall.transform.position = float3(0,0,-2);
		nearWall.material.emissive = float3(0,0,0);

		ParametricNode rightWall(NT_PLANE);
		setPlane(rightWall, float4(-1,0,0,0));
		rightWall.transform.position = float3(3,0,0);
		rightWall.material.emissive = float3(0,0,0.3f);
		rightWall.material.diffuse = float3(1,1,1);

		ParametricNode plane4(NT_PLANE);
		setPlane(plane4, float4(1,0,0,0));
		plane4.transform.position = float3(-1.3f,0,0);
		plane4.material.emissive = float3(0,0.0f,0.0f);
		plane4.material.diffuse = float3(0.1f,1,0.1f);
		
		ParametricNode sphere(NT_SPHERE);
		setSphereRadius(sphere, 0.5f);
		sphere.transform.position = float3(0,2, 3);
		float v = 2;
		sphere.material.emissive = float3(v,v,v);

		ParametricNode sphere2(NT_SPHERE);
		setSphereRadius(sphere2, 0.3f);
		sphere2.material.refraction = 1.0f;
		sphere2.transform.position = float3(-0.7f,0.4f, 3);
		sphere2.material.emissive = float3(0.0f,0,0);

		ParametricNode sphere3(NT_SPHERE);
		setSphereRadius(sphere3, 0.5f);
		sphere3.material.refraction = 1.0f;
		sphere3.transform.position = float3(0.9f,0.7f, 4.5f);
		sphere3.material.diffuse = float3(0.95, 0.95f, 1.0f);

		parametric_nodes.push_back(sphere);
		parametric_nodes.push_back(sphere2);
		parametric_nodes.push_back(sphere3);
		parametric_nodes.push_back(plane);
		parametric_nodes.push_back(farWall);
		parametric_nodes.push_back(rightWall);
		parametric_nodes.push_back(plane4);
		parametric_nodes.push_back(ceiling);
		parametric_nodes.push_back(nearWall);

		triangles = new Concurrency::array<Triangle, 1>(10);
	}

	static bool pick(const ray& r, IntersectInfo& info, IntersectInfo& tmp_info, const array_view<const ParametricNode, 1>& test_nodes) restrict(amp){
		bool hit = false;
		int cnt = test_nodes.get_extent().size();

		for(int i = 0; i < cnt; i++){
			if(intersect(test_nodes[i], r, tmp_info) && (!hit || info.distance > tmp_info.distance)){
				info = tmp_info;
				hit = true;
			}
		}
		return hit;
	}

	void Scene::clear(Tex2d* target2){
		Concurrency::graphics::writeonly_texture_view<Concurrency::graphics::direct3d::float4, 2> tv_new = *target2->AmpTextureView.get();
		array_view<float4, 2>& v1 = *trace_buffer_view;
		parallel_for_each(tv_new.accelerator_view, tv_new.extent.tile<TRACE_TS,TRACE_TS>().pad(), [v1](Concurrency::tiled_index<TRACE_TS,TRACE_TS> idx) restrict(amp){
			v1(idx) = 0;
		});
	}

    void Scene::trace(Tex2d* target2, int frame, int time){
		Concurrency::graphics::writeonly_texture_view<Concurrency::graphics::direct3d::float4, 2> tv_new = *target2->AmpTextureView.get(); 
		array_view<float4, 2>& v1 = *trace_buffer_view;

		array_view<Triangle, 1>& triangles = *triangle_buffer_view;
		



		cam.transform.position = float3(0,0.6f,0);
		cam.aspect = (float)target2->AmpTextureView->get_extent()[1] / (float)target2->AmpTextureView->get_extent()[0];
        
		array_view<Camera,1> camera(1, &cam);

		std::vector<float3> cam_corners_array;
		camera[0].ComputeCorners();
		for(int i = 0; i < 4; i++){
			cam_corners_array.push_back(camera[0].getCorner(i) - camera[0].transform.position);
		}
		array_view<float3, 1> cam_corners(cam_corners_array.size(), cam_corners_array);
		float3 viewRight = cam_corners_array[1] - cam_corners_array[0];
		float3 viewDown = cam_corners_array[2] - cam_corners_array[0];
		float3 viewTopLeft = cam_corners_array[0];
		float3 stepRight(viewRight / float(tv_new.extent[1]));
		float3 stepDown(viewDown / float(tv_new.extent[0]));

		array_view<const ParametricNode, 1> parametric_nodes_view(parametric_nodes.size(), parametric_nodes);
		int frameId = frame;
		frameId;
#define MAX_TRACE_DEPTH 5
		//
        parallel_for_each(tv_new.accelerator_view, tv_new.extent.tile<TRACE_TS,TRACE_TS>().pad(), [triangles, time, v1, viewRight, viewDown, viewTopLeft, cam_corners, camera, tv_new, parametric_nodes_view, frameId,stepRight,stepDown](Concurrency::tiled_index<TRACE_TS,TRACE_TS> idx) restrict(amp){
			ray rays;
			int tile_row = idx.local[0];
			int tile_col = idx.local[1];
			Camera* render_cam = &camera[0];
			float3 accumulated_color;
			float3 absorbed_color;
			
			//render screen size in pixels
			float scr_pix_width = (float)tv_new.extent[1];
			float scr_pix_height = (float)tv_new.extent[0];
			
			//screen space ray position
			float scr_ray_pix_pos_w = (float)idx.global[1];
			float scr_ray_pix_pos_h = (float)idx.global[0];

			float2 main_tc((float(idx.global[1])/scr_pix_width) * 2.0f - 1.0f, (float(idx.global[0])/scr_pix_height) * 2.0f - 1.0f);

			//compute initial camera ray
			rays.origin = render_cam->transform.position;
			rays.direction = viewTopLeft + (float(idx.global[1]) * stepRight) + (float(idx.global[0]) * stepDown);
			normalize((rays.direction));

			//initialize buffers
			//accumulated_colors[tile_row][tile_col] = float3(0,0,0);
			accumulated_color = float3(0,0,0);
			absorbed_color = float3(1,1,1);

			Material tri_material;
			
			

			//ray cast scene 
			for(int i = 0; i < MAX_TRACE_DEPTH; ++i){
				IntersectInfoBasic nearest_info;
				int nearest_id = -1;
				bool best_parametric = false;
				//intersect objects
				for(int j = 0; j < parametric_nodes_view.extent.size(); ++j){
					IntersectInfoBasic tmp_info;
					if(intersect_basic(parametric_nodes_view[j], rays, tmp_info)){
						if((nearest_id < 0) || (tmp_info.distance < nearest_info.distance)){
							nearest_info = tmp_info;
							nearest_id = j;
							best_parametric = true;
						}
					}
				}

				for(int j = 0; j < triangles.extent.size(); ++j){
					IntersectInfoBasic tri_tmp;
					if(RayTriangleIntersect(rays, triangles[j], tri_tmp)){
						if((nearest_id < 0) || (tri_tmp.distance < nearest_info.distance)){
							nearest_info = tri_tmp;
							best_parametric = false;
							nearest_id = j;
						}
					}
				}
				
				if(nearest_id < 0){ //no intersection - pick sky color
					accumulated_color += float3(0.1f,0.1f,0.1f) * absorbed_color;
					break;
				}else{
					Material mat;
					if(best_parametric)
						mat = parametric_nodes_view[nearest_id].material;
					else
						mat =  tri_material;


					accumulated_color += mat.emissive * absorbed_color;
					absorbed_color *= mat.diffuse;

					rays.origin = nearest_info.position + nearest_info.normal * 0.0001f;
					rays.direction = cosineWeightedDirection(float(time) / 900.0f  + float(i), nearest_info.normal,float3(main_tc.x, main_tc.y, 1.0f));
					normalize(rays.direction);
				}
			}

			v1(idx).rgb = lerp(v1(idx).rgb, accumulated_color, 1.0f / (1.0f + float(frameId)));
			v1(idx).w = 1;
			tv_new.set(idx, v1(idx));
        });
    }
}
