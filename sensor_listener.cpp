#include "sensor_listener.hpp"

#include <iostream>
#include <memory>

using boost::lockfree::queue;
using std::cout, std::cerr, std::endl;
using std::unique_ptr, std::make_unique;

SensorListener::SensorListener(int queue_size) {
    image_packet_queue = make_unique<queue<ImagePacket *>>(queue_size);
    if (image_packet_queue->is_lock_free()) {
        cout << "SensorListener image packet queue is lock-free" << endl;
    } else {
        cerr << "SensorListener image packet queue is not lock-free" << endl;
    }
}

void SensorListener::onSensorData(const RecFusion::ColorImage &color_img, const RecFusion::DepthImage &depth_img) {
    auto image_packet = new ImagePacket(color_img.width(), color_img.height(),
                                        depth_img.width(), depth_img.height());
    memcpy(image_packet->GetColorImage()->data(), color_img.data(), color_img.width() * color_img.height() * 3);
    memcpy(image_packet->GetDepthImage()->data(), depth_img.data(),
           depth_img.width() * depth_img.height() * sizeof(float));
    image_packet_queue->push(image_packet);
}
