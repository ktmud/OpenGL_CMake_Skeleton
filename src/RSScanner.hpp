/**
 * RSScanner.hpp
 */

#ifndef RSSCANNER_HEAD
#define RSSCANNER_HEAD

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

#include "system/Application.hpp"
#include "pointcloud/preview.hpp"

/// \class RSScanner
//  Initialize the RealSense Scanner app
class RSScanner : public Application
{
    public:
        RSScanner();

    protected:
        virtual void loop();

        // init point cloud view
        void init_pcview();

        void start_preview();
        void stop_preview();
        void render_pointcloud(float w, float h);

        void start_collect();
        void collect();
        void stop_collect();

    private:
        float time = 0.f;
        bool is_previewing = true;   // live previewing the point cloud
        bool is_collecting = false;  // collecting the stream and output a model
        bool device_ready = false;   // check whether device is ready

        pcview_state pcv;  // point cloud view state
        rs2::pipeline pipe;  // RealSense pipeline, encapsulating the actual device and sensors
        rs2::pointcloud pc;  // for calculating pointclouds and texture mappings
        rs2::points points;   // last obtained points
};

#endif /* end of include guard:RSSCANNER_HEAD */
