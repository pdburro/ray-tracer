cmake_minimum_required(VERSION 3.16)
project(RayTracer LANGUAGES CXX)

add_executable(ray_tracer src/main.cpp)
target_compile_features(ray_tracer PRIVATE cxx_std_17)
set_target_properties(ray_tracer PROPERTIES CXX_EXTENSIONS OFF)

if(MSVC)
    target_compile_options(ray_tracer PRIVATE /W4)
else()
    target_compile_options(ray_tracer PRIVATE -Wall -Wextra -Wpedantic)
endif()
