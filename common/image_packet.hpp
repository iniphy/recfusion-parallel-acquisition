#ifndef RECFUSION_PARALLEL_ACQUISITION_PROJECT_IMAGE_PACKET_HPP
#define RECFUSION_PARALLEL_ACQUISITION_PROJECT_IMAGE_PACKET_HPP

#include <RecFusion.h>
#include <memory>

class ImagePacket {

public:
    explicit ImagePacket(int color_width = 640, int color_height = 480,
                         int depth_width = 640, int depth_height = 480);

    ~ImagePacket() = default;

    std::unique_ptr<RecFusion::ColorImage> &GetColorImage() { return color_image; }

    std::unique_ptr<RecFusion::DepthImage> &GetDepthImage() { return depth_image; }

private:
    std::unique_ptr<RecFusion::ColorImage> color_image;
    std::unique_ptr<RecFusion::DepthImage> depth_image;

};


#endif //RECFUSION_PARALLEL_ACQUISITION_PROJECT_IMAGE_PACKET_HPP
