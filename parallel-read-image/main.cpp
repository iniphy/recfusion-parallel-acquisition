#include "timer.hpp"
#include "image_packet.hpp"

#include <boost/lockfree/queue.hpp>
#include <RecFusion.h>
#include <iostream>
#include <thread>
#include <vector>
#include <Windows.h>

typedef boost::lockfree::queue<ImagePacket*> ImagePacketQueue;

void AcquireImagePackets(RecFusion::Sensor* sensor, int timeout_ms, ImagePacketQueue* queue) {
    Timer timer;
    const int read_image_timeout_ms = 40;

    while (timer.elapsed() < timeout_ms) {
        auto image_packet = new ImagePacket(640, 480, 640, 480);
        if (sensor->readImage(*image_packet->GetDepthImage(), *image_packet->GetColorImage(), read_image_timeout_ms)) {
            if (!queue->push(image_packet)) {
                std::cout << "Failed to push image_packet" << std::endl;
            }
        } else {
            std::cout << "Failed to read images with timeout=" << read_image_timeout_ms << std::endl;
        }
    }
}

void CollectImagePackets(int num_sensor, std::vector<std::unique_ptr<ImagePacketQueue>>* queues, int timeout_ms) {
    Timer timer;
    std::vector<int> num_collected(num_sensor);
    bool something_left = false;
    while (timer.elapsed() < timeout_ms || something_left) {
        something_left = false;
        Sleep(15); // Simulate time for adding frames to the reconstruction object
        for (int sensor_id = 0; sensor_id < num_sensor; ++sensor_id) {
            if (timer.elapsed() >= timeout_ms && something_left) {
                std::cout << "Something left on sensor=" << sensor_id << std::endl;
            }
            auto& queue = (*queues)[sensor_id];
            ImagePacket* image_packet = nullptr;
            if (queue->pop(image_packet)) {
                something_left = true;
                ++num_collected[sensor_id];
                delete image_packet;
            }
        }
    }

    int total_collected = 0;
    for (int sensor_id = 0; sensor_id < num_sensor; ++sensor_id) {
        auto sensor_collected = num_collected[sensor_id];
        float avg = (float) timeout_ms / (float) sensor_collected;
        float fps = (float) sensor_collected / (float) timeout_ms * 1000.0f;
        std::cout << "On sensor with id=" << sensor_id
                  << " collected " << sensor_collected << " images "
                  << "(" << fps << " fps; "
                  << "avg time=" << avg << " ms)" << std::endl;
        total_collected += sensor_collected;
    }
    std::cout << "Collected in total " << total_collected << " images" << std::endl;
}

int main() {
    int num_sensor = 0;

    std::cout << "Using RecFusion 2.2.0 backend" << std::endl;
    RecFusion::RecFusionSDK::init();
    RecFusion::SensorManager sensor_manager;
    num_sensor = sensor_manager.deviceCount();

    std::cout << "Found " << num_sensor << " sensors" << std::endl;

    // Opening all available sensors
    std::vector<RecFusion::Sensor*> sensors;
    for (int sensor_id = 0; sensor_id < num_sensor; ++sensor_id) {
        RecFusion::Sensor* sensor;
        sensor = sensor_manager.sensor(sensor_id);
        if (!sensor->open()) {
            std::cout << "Failed to open sensor with id=" << sensor_id << std::endl;
        }
        sensors.push_back(sensor);
    }

    const int timeout_ms = 30000;
    std::cout << "Performing image reads per sensors for " << timeout_ms << " ms" << std::endl;

    // Create queues
    int queue_size = 900;
    std::vector<std::unique_ptr<ImagePacketQueue>> image_packet_queues;
    for (int sensor_id = 0; sensor_id < num_sensor; ++sensor_id) {
        image_packet_queues.emplace_back(std::make_unique<ImagePacketQueue>(queue_size));
    }

    std::vector<std::thread> collection_threads;
    for (int sensor_id = 0; sensor_id < num_sensor; ++sensor_id) {
        collection_threads.emplace_back(
                std::thread(AcquireImagePackets, sensors[sensor_id], timeout_ms + 200, image_packet_queues[sensor_id].get()));
    }
    collection_threads.emplace_back(std::thread(CollectImagePackets, num_sensor, &image_packet_queues, timeout_ms));
    for (auto& th : collection_threads) {
        th.join();
    }

    // Closing sensors
    for (int sensor_id = 0; sensor_id < num_sensor; ++sensor_id) {
        sensors[sensor_id]->stop();
        sensors[sensor_id]->close();
    }

    RecFusion::RecFusionSDK::deinit();
}
