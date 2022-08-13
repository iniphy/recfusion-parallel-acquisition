add_executable(parallel-listener
        main.cpp
        sensor_listener.hpp
        sensor_listener.cpp)

target_include_directories(parallel-listener BEFORE PUBLIC ${Boost_INCLUDE_DIRS})
target_link_libraries(parallel-listener common)
