#include "image_packet.hpp"

using RecFusion::ColorImage, RecFusion::DepthImage;
using std::make_unique;

ImagePacket::ImagePacket(int color_width/* = 640*/, int color_height/* = 480*/,
                         int depth_width/* = 640*/, int depth_height/* = 480*/) {
    color_image = make_unique<ColorImage>(color_width, color_height);
    depth_image = make_unique<DepthImage>(depth_width, depth_height);
}
