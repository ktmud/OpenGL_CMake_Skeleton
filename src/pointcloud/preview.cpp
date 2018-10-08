//
// Partially borrowed from https://github.com/IntelRealSense/librealsense
//
// License: Apache 2.0. See LICENSE file in root directory.
// Copyright(c) 2015 Intel Corporation. All Rights Reserved.

#ifndef RSSCANNER_POINTCLOUD_PREVIEW
#define RSSCANNER_POINTCLOUD_PREVIEW

#include "preview.hpp"

//////////////////////////////
// Basic Data Types         //
//////////////////////////////

// Handles all the OpenGL calls needed to display the point cloud
extern void draw_pointcloud(int width, int height, pcview_state& pc_state, rs2::points& points)
{
    if (!points)
        return;

    float _width = (float ) width;
    float _height = (float ) height;

    // OpenGL commands that prep screen for the pointcloud
    glPopMatrix();
    glPushAttrib(GL_ALL_ATTRIB_BITS);

    glClearColor(153.f / 255, 153.f / 255, 153.f / 255, 1);
    glClear(GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glm::perspective(glm::radians(60.f), _width/_height, 0.1f, 100.0f);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glm::lookAt(glm::vec3(0, 0, 0), glm::vec3(0, 0, 1), glm::vec3(0, -1, 0));

    glTranslatef(0, 0, +0.5f + pc_state.offset_y*0.05f);
    glRotated(pc_state.pitch, 1, 0, 0);
    glRotated(pc_state.yaw, 0, 1, 0);
    glTranslatef(0, 0, -0.5f);

    glPointSize(_width / 640);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, pc_state.tex.get_gl_handle());
    float tex_border_color[] = { 0.8f, 0.8f, 0.8f, 0.8f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, tex_border_color);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, 0x812F); // GL_CLAMP_TO_EDGE
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, 0x812F); // GL_CLAMP_TO_EDGE
    glBegin(GL_POINTS);


    /* this segment actually prints the pointcloud */
    auto vertices = points.get_vertices();              // get vertices
    auto tex_coords = points.get_texture_coordinates(); // and texture coordinates
    for (int i = 0; i < points.size(); i++)
    {
        if (vertices[i].z)
        {
            // upload the point and texture coordinates only for points we have depth data for
            glVertex3fv(vertices[i]);
            glTexCoord2fv(tex_coords[i]);
        }
    }

    // OpenGL cleanup
    glEnd();
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glPopAttrib();
    glPushMatrix();
}

// Update state for point cloud view
extern void update_pc_state(pcview_state& pc_state)
{
    ImGuiIO& io = ImGui::GetIO();
    pc_state.ml = ImGui::IsMouseDown(0); // is left mouse down?
    pc_state.offset_x = -io.MouseWheelH;
    pc_state.offset_y = -io.MouseWheel;

    ImVec2 pos = io.MousePos;
    double x = pos.x;
    double y = pos.y;
    if (pc_state.ml)
    {
        pc_state.yaw -= (x - pc_state.last_x);
        pc_state.yaw = std::max(pc_state.yaw, -120.0);
        pc_state.yaw = std::min(pc_state.yaw, +120.0);
        pc_state.pitch += (y - pc_state.last_y);
        pc_state.pitch = std::max(pc_state.pitch, -80.0);
        pc_state.pitch = std::min(pc_state.pitch, +80.0);
    }
    pc_state.last_x = x;
    pc_state.last_y = y;

    // Escape key resets the view
    if (ImGui::IsKeyReleased(32))
    {
        pc_state.yaw = pc_state.pitch = 0;
        pc_state.offset_x = pc_state.offset_y = 0.0;
    }
}

#endif /* end of include guard: RSSCANNER_POINTCLOUD_PREVIEW */