#ifndef RECFUSION_PARALLEL_ACQUISITION_PROJECT_SENSOR_LISTENER_HPP
#define RECFUSION_PARALLEL_ACQUISITION_PROJECT_SENSOR_LISTENER_HPP

#include "image_packet.hpp"
#include <RecFusion.h>
#include <boost/lockfree/queue.hpp>

class SensorListener : public RecFusion::SensorListener {

public:
    explicit SensorListener(int queue_size);

    void onSensorData(const RecFusion::ColorImage &colorImg, const RecFusion::DepthImage &depthImg) override;

    std::unique_ptr<boost::lockfree::queue<ImagePacket *>> &GetImagePacketQueue() { return image_packet_queue; }

private:
    std::unique_ptr<boost::lockfree::queue<ImagePacket *>> image_packet_queue;
};


#endif //RECFUSION_PARALLEL_ACQUISITION_PROJECT_SENSOR_LISTENER_HPP
