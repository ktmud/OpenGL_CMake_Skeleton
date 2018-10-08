//
// Partially borrowed from https://github.com/IntelRealSense/librealsense
//
// License: Apache 2.0. See LICENSE file in root directory.
// Copyright(c) 2015 Intel Corporation. All Rights Reserved.

#ifndef RSSCANNER_POINTCLOUD_PREVIEW_H
#define RSSCANNER_POINTCLOUD_PREVIEW_H

#include <string>
#include <sstream>
#include <iostream>
#include <algorithm>

#include <GLFW/glfw3.h>

#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective

#include <librealsense2/rs.hpp> // Include RealSense Cross Platform API

#include "imgui.h"

//////////////////////////////
// Basic Data Types         //
//////////////////////////////

struct float3 { float x, y, z; };
struct float2 { float x, y; };

struct rect
{
    float x, y;
    float w, h;

    // Create new rect within original boundaries with give aspect ration
    rect adjust_ratio(float2 size) const
    {
        auto H = static_cast<float>(h), W = static_cast<float>(h) * size.x / size.y;
        if (W > w)
        {
            auto scale = w / W;
            W *= scale;
            H *= scale;
        }

        return{ x + (w - W) / 2, y + (h - H) / 2, W, H };
    }
};

////////////////////////
// Image display code //
////////////////////////
class texture
{
public:
    void render(const rs2::frameset& frames, int window_width, int window_height)
    {
        std::vector<rs2::video_frame> supported_frames;
        for (auto f : frames)
        {
            if (can_render(f))
                supported_frames.push_back(f);
        }
        if (supported_frames.empty())
            return;

        std::sort(supported_frames.begin(), supported_frames.end(), [](rs2::frame first, rs2::frame second)
            { return first.get_profile().stream_type() < second.get_profile().stream_type();  });

        auto image_grid = calc_grid(float2{ (float)window_width, (float)window_height }, supported_frames);

        int image_index = 0;
        for (auto f : supported_frames)
        {
            upload(f);
            show(image_grid.at(image_index));
            image_index++;
        }
    }

    void render(const rs2::video_frame& frame, const rect& r)
    {
        upload(frame);
        show(r.adjust_ratio({ float(width), float(height) }));
    }

    void upload(const rs2::video_frame& frame)
    {
        if (!frame) return;

        if (!gl_handle)
            glGenTextures(1, &gl_handle);
        GLenum err = glGetError();

        auto format = frame.get_profile().format();
        width = frame.get_width();
        height = frame.get_height();
        stream = frame.get_profile().stream_type();

        glBindTexture(GL_TEXTURE_2D, gl_handle);

        switch (format)
        {
        case RS2_FORMAT_RGB8:
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, frame.get_data());
            break;
        case RS2_FORMAT_RGBA8:
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, frame.get_data());
            break;
        case RS2_FORMAT_Y8:
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, frame.get_data());
            break;
        default:
            throw std::runtime_error("The requested format is not supported by this demo!");
        }

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    GLuint get_gl_handle() { return gl_handle; }

    void show(const rect& r) const
    {
        if (!gl_handle)
            return;

        glBindTexture(GL_TEXTURE_2D, gl_handle);
        glEnable(GL_TEXTURE_2D);
        glBegin(GL_QUAD_STRIP);
        glTexCoord2f(0.f, 1.f); glVertex2f(r.x, r.y + r.h);
        glTexCoord2f(0.f, 0.f); glVertex2f(r.x, r.y);
        glTexCoord2f(1.f, 1.f); glVertex2f(r.x + r.w, r.y + r.h);
        glTexCoord2f(1.f, 0.f); glVertex2f(r.x + r.w, r.y);
        glEnd();
        glDisable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);

        //  draw_text((int)r.x + 15, (int)r.y + 20, rs2_stream_to_string(stream));
        ImGui::Text(rs2_stream_to_string(stream), (int)r.x + 15, (int)r.y + 20);
    }

private:
    GLuint gl_handle = 0;
    int width = 0;
    int height = 0;
    rs2_stream stream = RS2_STREAM_ANY;

    bool can_render(const rs2::frame& f) const
    {
        auto format = f.get_profile().format();
        switch (format)
        {
        case RS2_FORMAT_RGB8:
        case RS2_FORMAT_RGBA8:
        case RS2_FORMAT_Y8:
            return true;
        default:
            return false;
        }
    }

    rect calc_grid(float2 window, size_t streams)
    {
        if (window.x <= 0 || window.y <= 0 || streams <= 0)
            throw std::runtime_error("invalid window configuration request, failed to calculate window grid");
        float ratio = window.x / window.y;
        auto x = sqrt(ratio * (float)streams);
        auto y = (float)streams / x;
        auto w = round(x);
        auto h = round(y);
        if (w == 0 || h == 0)
            throw std::runtime_error("invalid window configuration request, failed to calculate window grid");
        while (w*h > streams)
            h > w ? h-- : w--;
        while (w*h < streams)
            h > w ? w++ : h++;
        auto new_w = round(window.x / w);
        auto new_h = round(window.y / h);
        return rect{ w, h, new_w, new_h}; //column count, line count, cell width cell height
    }

    std::vector<rect> calc_grid(float2 window, std::vector<rs2::video_frame>& frames)
    {
        auto grid = calc_grid(window, frames.size());

        int index = 0;
        std::vector<rect> rv;
        int curr_line = -1;
        for (auto f  : frames)
        {
            auto mod = index % (int)grid.x;
            auto fw = (float)frames[index].get_width();
            auto fh = (float)frames[index].get_height();

            float cell_x_postion = (float)(mod * grid.w);
            if (mod == 0) curr_line++;
            float cell_y_position = curr_line * grid.h;

            auto r = rect{ cell_x_postion, cell_y_position, grid.w, grid.h };
            rv.push_back(r.adjust_ratio(float2{ fw, fh }));
            index++;
        }

        return rv;
    }
};


// Struct for managing rotation of pointcloud view
struct pcview_state {
    pcview_state() : yaw(15.0), pitch(15.0), last_x(0.0), last_y(0.0),
        ml(false), offset_x(2.f), offset_y(2.f), tex() {}
    double yaw;
    double pitch;
    double last_x;
    double last_y;
    bool ml;
    float offset_x;
    float offset_y;
    texture tex;
};


// Handles all the OpenGL calls needed to display the point cloud
extern void draw_pointcloud(int width, int height, pcview_state& pc_state, rs2::points& points);

// Update state for point cloud view
extern void update_pc_state(pcview_state& pc_state);

#endif /* end of include guard: RSSCANNER_POINTCLOUD_PREVIEW_H */