#include "timer.hpp"
#include "sensor_listener.hpp"

#include <RecFusion.h>
#include <thread>
#include <vector>

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
        bool opening_success;
        RecFusion::Sensor* sensor;
        sensor = sensor_manager.sensor(sensor_id);
        opening_success = sensor->open();
        sensors.push_back(sensor);
        if (!opening_success) {
            std::cout << "Failed to open sensor with id=" << sensor_id << std::endl;
        }
    }

    const int timeout_ms = 5000;
    std::cout << "Performing image reads per sensors for " << timeout_ms << " ms" << std::endl;
    // TODO: technically, we don't need additional thread here :)
    std::thread reconstruction([=]() {
        Timer timer;
        std::vector<std::unique_ptr<SensorListener>> listeners;
        std::vector<int> num_collected(num_sensor);

        for (int sensor_id = 0; sensor_id < num_sensor; ++sensor_id) {
            listeners.push_back(std::make_unique<SensorListener>(5 * 30));
            sensors[sensor_id]->addListener(listeners[sensor_id].get());
            sensors[sensor_id]->start();
        }

        while (timer.elapsed() < timeout_ms) {
            // Do the reconstruction
            for (int sensor_id = 0; sensor_id < num_sensor; ++sensor_id) {
                auto& image_packet_queue = listeners[sensor_id]->GetImagePacketQueue();
                ImagePacket* image_packet;
                while(!image_packet_queue->pop(image_packet) && timer.elapsed() < timeout_ms) {}
                if (image_packet != nullptr) {
                    num_collected[sensor_id]++;
                    delete image_packet;
                }
            }
        }
        // TODO: handle frames available after the acquisition timeout!
        int total_collected = 0;
        for (int sensor_id = 0; sensor_id < num_sensor; ++sensor_id) {
            auto sensor_collected = num_collected[sensor_id];
            float fps = (float) sensor_collected / timeout_ms / 1000.0f;
            float avg = (float) timeout_ms / (float) sensor_collected;
            std::cout << "On sensor with id=" << sensor_id
                      << " collected " << sensor_collected << " images "
                      << "(" << fps << " fps; "
                      << "avg time=" << avg << " ms)" << std::endl;
            total_collected += sensor_collected;
        }

        std::cout << "Collected in total " << total_collected << " images" << std::endl;

        for (int sensor_id = 0; sensor_id < num_sensor; ++sensor_id) {
            sensors[sensor_id]->stop();
            sensors[sensor_id]->removeListener(listeners[sensor_id].get());
        }
    });
    reconstruction.join();

    // Closing sensors
    for (int sensor_id = 0; sensor_id < num_sensor; ++sensor_id) {
        sensors[sensor_id]->close();
    }

    RecFusion::RecFusionSDK::deinit();
}