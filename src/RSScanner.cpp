/**
 * RSScanner.cpp
 */

#ifndef RSSCANNER_APP_CPP
#define RSSCANNER_APP_CPP

#include <vector>
#include <iostream>
#include "utils/glError.hpp"

#include "RSScanner.hpp"

RSScanner::RSScanner():
    Application()
{
    init_pcview();  // init point cloud viewport
    glCheckError(__FILE__, __LINE__);
}

void RSScanner::init_pcview()
{

}

void RSScanner::loop()
{
    if (is_live) {
        // update point cloud state
        // update_pc_state(pc_state);
    }

    // Render ImGui controls
    ImGui::Begin("Control Streaming");
    if (ImGui::Button("Start")) {
        pipe.start();

        // Wait for the next set of frames from the camera
        auto frames = pipe.wait_for_frames();
        auto depth = frames.get_depth_frame();

        // Generate the pointcloud and texture mappings
        points = pc.calculate(depth);
        auto color = frames.get_color_frame();

        // For cameras that don't have RGB sensor, we'll map the pointcloud to infrared instead of color
        if (!color)
            color = frames.get_infrared_frame();

        // Tell pointcloud object to map to this color frame
        pc.map_to(color);

        // Upload the color frame to OpenGL
        pc_state.tex.upload(color);

        // Draw the pointcloud
        draw_pointcloud(getWidth(), getHeight(), pc_state, points);
    }
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                 1000.0f / ImGui::GetIO().Framerate,
                 ImGui::GetIO().Framerate);
    ImGui::End();

}


#endif /* end of include guard: RSSCANNER_APP_CPP */
