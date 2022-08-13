add_library(common
        image_packet.hpp
        image_packet.cpp
        timer.hpp)

target_include_directories(common BEFORE PUBLIC ${RECFUSION_220_PATH}/include .)
target_link_directories(common BEFORE PUBLIC ${RECFUSION_220_PATH}/lib)
target_link_libraries(common RecFusionSDK)
