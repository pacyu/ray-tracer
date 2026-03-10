cmake_minimum_required(VERSION 3.10)
project(rayt)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(OpenCV_DIR "C:/OpenCV/build")

# 使用 CMAKE_SOURCE_DIR 确保路径相对于项目根目录 
if(MSVC)
    # 针对 Visual Studio 等多配置生成器，取消 Debug/Release 子目录
    foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
        string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG)
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} "${CMAKE_SOURCE_DIR}/bin")
        set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} "${CMAKE_SOURCE_DIR}/lib")
        set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} "${CMAKE_SOURCE_DIR}/lib")
    endforeach()
else()
    # 针对 Makefile/Ninja 等单配置生成器
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/lib")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/lib")
endif()

# 依赖项
find_package(OpenMP REQUIRED)
find_package(OpenCV REQUIRED)

include_directories(include)

# 构建静态库
file(GLOB_RECURSE SOURCES "src/parser/*.cpp" "src/core/*.cpp")
add_library(tracer_lib ${SOURCES})
target_link_libraries(tracer_lib PRIVATE ${OpenCV_LIBS} OpenMP::OpenMP_CXX)

# 构建主可执行文件
add_executable(${PROJECT_NAME} src/main.cpp)
target_include_directories(${PROJECT_NAME} PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE tracer_lib)

# 构建解析器测试可执行文件
add_executable(test_parser tests/test_parser.cpp)
target_include_directories(test_parser PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(test_parser PRIVATE tracer_lib)

# 构建渲染器测试可执行文件
add_executable(test_tracer tests/test_tracer.cpp)
target_include_directories(test_tracer PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(test_tracer PRIVATE tracer_lib)

# 启用 IPO（Interprocedural Optimization）以提高性能
include(CheckIPOSupported)
check_ipo_supported(RESULT result OUTPUT error)
if(result)
    set_target_properties(${PROJECT_NAME} PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()
