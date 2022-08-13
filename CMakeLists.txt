cmake_minimum_required(VERSION 3.0)

project(recfusion-parallel-acquisition-project VERSION "1.0.0" LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin/)

# From: https://stackoverflow.com/a/60844772/1319478
if(MSVC)
    add_definitions(-D_WIN32_WINNT=0x600)
    add_compile_options(
        $<$<CONFIG:>:/MT> #---------|
        $<$<CONFIG:Debug>:/MTd> #---|-- Statically link the runtime libraries
        $<$<CONFIG:Release>:/MT> #--|
)
endif()

# Hint for the Boost lib if it isn't installed correctly
set(BOOST_ROOT "E:\\Boost\\boost_1_79_0")
set(RECFUSION_220_PATH "C:\\Program Files\\ImFusion\\RecFusion SDK 2.2.0")

find_package(Boost REQUIRED)

add_subdirectory(common)
add_subdirectory(parallel-listener)
add_subdirectory(parallel-read-image)
