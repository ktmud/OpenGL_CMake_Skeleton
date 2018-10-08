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

        void init_pcview();
        void start_preview();
        void stop_preview();
        void start_collect();
        void stop_collect();

    private:
        float time = 0.f;
        bool is_live = true;  // live previewing the point cloud
        bool is_saving = false;  // collecting the stream and output a model
        pcview_state pc_state;  // point cloud state

        // Declare pointcloud object, for calculating pointclouds and texture mappings
        rs2::pointcloud pc;
        // We want the points object to be persistent so we can display the last cloud when a frame drops
        rs2::points points;
        // Declare RealSense pipeline, encapsulating the actual device and sensors
        rs2::pipeline pipe;
};

#endif /* end of include guard:RSSCANNER_HEAD */
