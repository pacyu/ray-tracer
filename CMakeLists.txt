cmake_minimum_required(VERSION 3.10)
project(raytracer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(OpenCV_DIR "C:/OpenCV/build")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin)

find_package(OpenMP REQUIRED)

find_package(OpenCV REQUIRED)

add_executable(${PROJECT_NAME}
  src/main.cpp
  src/heart.h
  src/sphere.h
)

target_include_directories(${PROJECT_NAME} PRIVATE src)

target_link_libraries(${PROJECT_NAME}
  PRIVATE
  OpenMP::OpenMP_CXX
  ${OpenCV_LIBS}
)