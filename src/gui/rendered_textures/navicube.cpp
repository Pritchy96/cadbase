#include <glm/common.hpp>
#include <glm/geometric.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/fwd.hpp>
#include <glm/ext.hpp>
#include <glm/ext/quaternion_trigonometric.hpp>

#include <spdlog/spdlog.h>

#include <memory>
#include <vector>

#include "cad-base/gui/rendered_textures/navicube.hpp"
#include "cad-base/gui/rendered_textures/gui_render_texture.hpp"
#include "cad-base/raycast/ray.hpp"
#include "cad-base/viewport_input.hpp"

using std::shared_ptr;
using std::make_shared;
using std::make_unique;

using glm::vec3;    

NaviCube::NaviCube(GLFWwindow *window, glm::vec4 background_col, int window_width, int window_height, std::vector<std::shared_ptr<Camera>> affected_cameras) 
    : GuiRenderTexture(window, background_col, window_width, window_height),
      ViewportInput(affected_cameras) {

    camera->SetProjectionStyle(true);
    camera->can_pan = false;
    camera->can_zoom = false;
    this->affected_cameras.emplace_back(camera);

    shared_ptr<Geometry> geo = make_shared<Geometry>(face_1_, "Face 1");
	geo_renderable_pairs.emplace_back(geo, make_unique<Renderable>(basic_shader, geo, GL_TRIANGLES));

    geo = make_shared<Geometry>(face_2_, "Face 2");
	geo_renderable_pairs.emplace_back(geo, make_unique<Renderable>(basic_shader, geo, GL_TRIANGLES));
    
    geo = make_shared<Geometry>(face_3_, "Face 3");
	geo_renderable_pairs.emplace_back(geo, make_unique<Renderable>(basic_shader, geo, GL_TRIANGLES));

    geo = make_shared<Geometry>(face_4_, "Face 4");
	geo_renderable_pairs.emplace_back(geo, make_unique<Renderable>(basic_shader, geo, GL_TRIANGLES));

    geo = make_shared<Geometry>(face_5_, "Face 5");
	geo_renderable_pairs.emplace_back(geo, make_unique<Renderable>(basic_shader, geo, GL_TRIANGLES));

    geo = make_shared<Geometry>(face_6_, "Face 6");
	geo_renderable_pairs.emplace_back(geo, make_unique<Renderable>(basic_shader, geo, GL_TRIANGLES));
}

void NaviCube::DeselectRenderable() {
    // Handle de-selecting previous selection before selecting new one.
    if (selected_face_ != nullptr) {
        selected_face_->geometry->draw_aa_bounding_box = false;
        selected_face_ = nullptr;
    }
}

//From: https://github.com/opengl-tutorials/ogl/blob/master/common/quaternion_utils.cpp
//TODO: Why does this work, but camera->SetRotation(glm::rotate(camera->GetRotation(), angle, cross_vector)); doesn't?
glm::quat NaviCube::RotationBetweenVectors(vec3 start, vec3 dest) {
	start = normalize(start);
	dest = normalize(dest);

	float cos_theta = dot(start, dest);
	vec3 rotation_axis;

	if (cos_theta < -1 + 0.001f){
		// Special case when vectors in opposite directions - there is no "ideal" rotation axis
		// So guess one; any will do as long as it's perpendicular to start
		rotation_axis = cross(vec3(0.0f, 0.0f, 1.0f), start);
		if (glm::length2(rotation_axis) < 0.01 ) //Parallel, try again
			rotation_axis = cross(vec3(1.0f, 0.0f, 0.0f), start);

		rotation_axis = normalize(rotation_axis);
		return glm::angleAxis(glm::radians(180.0f), rotation_axis);
	}

	rotation_axis = glm::cross(start, dest);

    //Optimised Implementation from Stan Melax's Game Programming Gems 1 article
	float s = sqrt( (1+cos_theta)*2 );
	float invs = 1 / s;

	return {
		s * 0.5f, 
		rotation_axis.x * invs,
		rotation_axis.y * invs,
		rotation_axis.z * invs
	};
}

void NaviCube::SelectRenderable(std::shared_ptr<Renderable> selected_renderable) {    
    selected_face_ = selected_renderable;
    spdlog::info("Selected NaviCube Face: {0}", selected_renderable->geometry->name); 

    glm::vec3 camera_forward = glm::normalize(camera->GetCameraTransform()[2]);
    glm::vec3 camera_up = camera->GetCameraTransform()[1];

    //Some (kinda dumb) maths to figure out the surface normal of the face, taking advantage of the fact that the navicube is just a box with center (0, 0, 0)
    //also figure out the new up direction 
    glm::vec3 summed_triangle = selected_face_->geometry->vertexes.at(0) + selected_face_->geometry->vertexes.at(1) + selected_face_->geometry->vertexes.at(2);
    glm::vec3 surface_normal;

    //Cardinal direction of up vector of camera.
    //We use this to snap to the nearest cardinal direction as our new up vector.
    glm::vec3 new_up;
    glm::vec3 camera_up_abs = glm::abs(camera_up);

    if (std::abs(summed_triangle.x) == (3 * CUBE_SIZE)) {
        surface_normal = glm::normalize(glm::vec3(summed_triangle.x, 0.0f, 0.0f));

        if (camera_up_abs.y > camera_up_abs.z) {
            new_up = glm::normalize(glm::vec3(0.0f, camera_up.y, 0.0f));
        } else {
            new_up = glm::normalize(glm::vec3(0.0f, 0.0f, camera_up.z));
        }
    } else if (std::abs(summed_triangle.y) == (3 * CUBE_SIZE)) {
        surface_normal = glm::normalize(glm::vec3(0.0f, summed_triangle.y, 0.0f));

        if (camera_up_abs.x > camera_up_abs.z) {
            new_up = glm::normalize(glm::vec3(camera_up.x, 0.0f, 0.0f));
        } else {
            new_up = glm::normalize(glm::vec3(0.0f, 0.0f, camera_up.z));
        }
    } else {
        surface_normal = glm::normalize(glm::vec3(0.0f, 0.0f, summed_triangle.z));

        if (camera_up_abs.x > camera_up_abs.y) {
            new_up = glm::normalize(glm::vec3(camera_up.x, 0.0f, 0.0f));
        } else {
            new_up = glm::normalize(glm::vec3(0.0f, camera_up.y, 0.0f));
        }
    }

    //Rotate camera so that it's normal is perpendicular to clicked face.
    glm::quat camera_rotation_quat = RotationBetweenVectors(camera_forward, surface_normal);

    // Set up vector to closest cardinal direction to camera original up.
    vec3 current_up = camera_rotation_quat * camera_up;
	glm::quat up_vector_quat = RotationBetweenVectors(current_up, new_up);
	
	// Generate new rotation
    for (std::shared_ptr<Camera> c : affected_cameras) {
        c->SLERPCameraRotation(up_vector_quat * camera_rotation_quat * glm::quat(camera->GetRotation()), 0.2f);
    }

}