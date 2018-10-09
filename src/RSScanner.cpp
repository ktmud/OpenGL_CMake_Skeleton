/**
 * RSScanner.cpp
 */

#ifndef RSSCANNER_APP_CPP
#define RSSCANNER_APP_CPP

#include <vector>
#include <iostream>
#include <thread>

#include "utils/glError.hpp"
#include "RSScanner.hpp"

using namespace std;

RSScanner::RSScanner():
    Application()
{
    init_pcview();  // init point cloud viewport
    glCheckError(__FILE__, __LINE__);
}

void RSScanner::init_pcview()
{
    // const auto CAPACITY = 5; // allow max latency of 5 frames
    // queue = rs2::frame_queue(CAPACITY);

    // std::thread t([&]() {
    //     while (true)
    //     {
    //         rs2::depth_frame frame;
    //         if (queue.poll_for_frame(&frame))
    //         {
    //             frame.get_data();
    //             // Do processing on the frame
    //         }
    //     }
    // });
    // t.detach();

    if (is_previewing)
    {
        start_preview();
    }
}

void RSScanner::start_preview()
{
    is_previewing = true;
    rs2::context ctx;
    auto list = ctx.query_devices(); // Get a snapshot of currently connected devices
    if (list.size() > 0) 
    {
        device_ready = true;
        // update pipeline
        pipe = rs2::pipeline(ctx);
        pipe.start();
    } else {
        device_ready = false;
    }
}
void RSScanner::stop_preview()
{
    is_previewing = false;
    if (device_ready) {
        pipe.stop();
    }
}

void RSScanner::render_pointcloud(float w, float h)
{
    if (!device_ready)
    {
        ImGui::Text("No device detected. Is it plugged in?");
        return;
    }

    update_pc_state(pcv);

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
    pcv.tex.upload(color);
    
    // Draw the pointcloud in texture context
    draw_pointcloud(w, h, pcv, points);
    ImVec2 pos = ImGui::GetCursorPos();
    auto win_lt = ImGui::GetItemRectMin();  // window left top
    auto rel_lt = ImGui::GetWindowContentRegionMin();  // content relative to window
    auto rel_br = ImGui::GetWindowContentRegionMax();  // max pos of content relative to window

    // Ask ImGui to draw it as an image:
    // Under OpenGL the ImGUI image type is GLuint
    // So make sure to use "(void *)tex" but not "&tex"
    ImGui::GetWindowDrawList()->AddImage(
        (void *)pcv.tex.get_gl_handle(),
        ImVec2(win_lt.x + rel_br.x, win_lt.y + rel_br.y),
        ImVec2(win_lt.x + rel_lt.x, win_lt.y + rel_lt.y),
        ImVec2(0, 1),
        ImVec2(1, 0));
}

void RSScanner::loop()
{
    auto io = ImGui::GetIO();
    ImGuiViewport* viewport = ImGui::GetMainViewport(); 
    ImVec2 pos = ImVec2(viewport->Pos.x, viewport->Pos.y);
    float gut = 5.f;  // gutters between windows

    ImGuiWindowFlags flags_tooltip = 0;
    ImGuiWindowFlags flags_default = 0;

    flags_tooltip |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize;
    flags_tooltip |= ImGuiWindowFlags_NoMove;

    ImGui::SetNextWindowBgAlpha(0.3f);
    ImGui::SetNextWindowSize(ImVec2(200.f, 20.f), ImGuiCond_Once);
    ImGui::SetNextWindowPos(ImVec2(pos[0] + gut, pos[1] + gut), ImGuiCond_Once);
    ImGui::Begin("FPS", false, flags_tooltip);
    ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
    ImGui::End();

    // Render ImGui controls
    auto size = ImGui::GetWindowSize();
    ImGui::SetNextWindowPos(ImVec2(pos[0] + size[0] + gut, pos[1] + gut), ImGuiCond_Always);
    ImGui::Begin("Control Streaming", false, flags_tooltip);

    if (is_previewing) {
        // if (int(ImGui::GetTime()) % 2 == 0)
        // {
        //     // check device availability every 2 second
        //     start_preview();
        // }
        if (ImGui::Button("Stop")) {
            stop_preview();
        }
    } else {
        if (ImGui::Button("Start")) {
            start_preview();
        }
    }
    ImGui::End();

    if (is_previewing) {
        ImGui::SetNextWindowPos(ImVec2(pos[0] + viewport->Size.x * 0.5f - 300.f,
                                       pos[1] + gut + 50.f),
                                ImGuiCond_Once);
        float w = 620.f;
        float h = 480.f;
        ImGui::SetNextWindowSize(ImVec2(w, h), ImGuiCond_Once);
        ImGui::Begin("Preview");
        render_pointcloud(w, h);
        ImGui::End();
    }
}


#endif /* end of include guard: RSSCANNER_APP_CPP */
