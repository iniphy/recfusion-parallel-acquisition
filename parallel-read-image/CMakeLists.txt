add_executable(parallel-read-image main.cpp)
target_include_directories(parallel-read-image BEFORE PUBLIC ${Boost_INCLUDE_DIRS})
target_link_libraries(parallel-read-image RecFusionSDK common)
